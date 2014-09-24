#import "PTSCalendarListViewController.h"
#import <PebbleKit/PebbleKit.h>
#import <EventKit/EventKit.h>
#import "UIImage+PTSFoundation.h"
#import "PTSNotificationView.h"

@interface PTSCalendarListViewController()<
PBPebbleCentralDelegate,
PBWatchDelegate
>
@property (nonatomic, strong) EKEventStore *eventStore;
@property (nonatomic, strong) NSArray *calendars;
@property (nonatomic, strong) NSMutableSet *selectedCalendars;
@property (nonatomic, strong) PBWatch *connectedWatch;
@property (nonatomic, strong) NSMutableArray *messagingQueue;
@property (nonatomic, assign) BOOL isSending;
@property (nonatomic, assign) NSInteger failureCount;
@property (nonatomic, strong) UIView *processingView;
@end

#define PEBBLE_APP_MESSAGE_SIZE_MAX 128

#define EVENT_ID_KEY @0
#define EVENT_TITLE_KEY @1
#define EVENT_TITLE_IMAGE_KEY @2
#define EVENT_START_KEY @3
#define EVENT_END_KEY @4
#define EVENT_IMAGE_WIDTH @5
#define EVENT_IMAGE_HEIGHT @6
#define EVENT_IMAGE_ROW_SIZE_BYTES @7
#define EVENT_IMAGE_INFO_FLAGS @8

@implementation PTSCalendarListViewController

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Custom initialization
        [self initialize];
    }
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    // Do any additional setup after loading the view from its nib.
    UIEdgeInsets insets = self.tableView.contentInset;
    insets.bottom = self.sendButton.frame.size.height;
    [self.tableView setContentInset:insets];
    [self.tableView setScrollIndicatorInsets:insets];
    
    [self.sendButton setTitle:NSLocalizedString(@"Send today's events to Pebble", nil) forState:UIControlStateNormal];
    
    __weak __typeof(self) weakSelf = self;
    [self.eventStore requestAccessToEntityType:EKEntityTypeEvent completion:^(BOOL granted, NSError *error) {
        if (granted && !error) {
            weakSelf.calendars = [weakSelf.eventStore calendarsForEntityType:EKEntityTypeEvent];
            [weakSelf.selectedCalendars addObjectsFromArray:weakSelf.calendars];
            [weakSelf.tableView reloadData];
            dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(0.2 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
                [weakSelf.tableView reloadData];
            });
        }
    }];
}

- (void)initialize {
    self.title = NSLocalizedString(@"AppName", nil);
    self.navigationItem.prompt = NSLocalizedString(@"Select calendars to send to Pebble", nil);
    self.selectedCalendars = [[NSMutableSet alloc] init];
    self.messagingQueue = [NSMutableArray array];
    self.failureCount = 0;
    
    [[PBPebbleCentral defaultCentral] setDelegate:self];
    [[PBPebbleCentral defaultCentral] setAppUUID:[self createUUID]];
    self.connectedWatch = [[PBPebbleCentral defaultCentral] lastConnectedWatch];
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

#pragma mark - Public Methods
- (void)setConnectedWatch:(PBWatch *)connectedWatch {
    _connectedWatch.delegate = nil;
    _connectedWatch = connectedWatch;
    _connectedWatch.delegate = self;
}

#pragma mark - Private Methods
- (NSData*)createUUID {
    uint8_t bytes[] = {0x26, 0x86, 0x04, 0x64, 0xfb, 0x51, 0x40, 0x06, 0x98, 0xc0, 0x9b, 0x31, 0x40, 0xb2, 0x8e, 0x8a};
    return [NSData dataWithBytes:bytes length:sizeof(bytes)];
}

- (EKEventStore*)eventStore {
    if (!_eventStore) {
        _eventStore = [[EKEventStore alloc] init];
    }
    return _eventStore;
}

- (void)sendAppMessageFromQueue {
    if (self.messagingQueue.count == 0 || 3 < self.failureCount) {
        [self.connectedWatch closeSession:nil];
        [self endProcessing];
        return;
    }
    
    [self startProcessing];
    
    [self.connectedWatch appMessagesGetIsSupported:^(PBWatch *watch, BOOL isAppMessagesSupported) {
        if (isAppMessagesSupported) {
            NSDictionary *firstEvent = self.messagingQueue.firstObject;
            LOG(@"[send AppMessage]:%@", firstEvent);

            NSString *sendingMessage = [NSString stringWithFormat:@"%@", firstEvent[EVENT_TITLE_KEY]];
            double timestamp = [firstEvent[EVENT_START_KEY] doubleValue];
            NSUInteger timezoneSec = [[NSTimeZone systemTimeZone] secondsFromGMT];
            NSDate *startDate = [NSDate dateWithTimeIntervalSince1970:timestamp - timezoneSec];

            PTSNotificationView *notificationView = [[PTSNotificationView alloc] initWithDate:startDate message:sendingMessage];
            [notificationView show];
            
            NSMutableDictionary *event = [NSMutableDictionary dictionaryWithDictionary:firstEvent];
            [event removeObjectForKey:EVENT_TITLE_KEY];

            [self.connectedWatch appMessagesPushUpdate:firstEvent onSent:^(PBWatch *watch, NSDictionary *update, NSError *error) {
                float nextDuration;
                self.isSending = NO;
                if (!error) {
                    // success
                    LOG(@"[Success!]send AppMessage:%@", update);
                    [self.messagingQueue removeObject:firstEvent];
                    nextDuration = 0.25;
                    
                    [notificationView dismissWithSuccess];
                } else {
                    // failure
                    self.failureCount++;
                    LOG(@"[Failure!]send AppMessage error:%@", error);
                    nextDuration = 0.5;

                    [notificationView dismissWithFailure];
                }
                dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(nextDuration * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
                    [self sendAppMessageFromQueue];
                });
            }];
        } else {
            self.isSending = NO;
            
            NSString *message = [NSString stringWithFormat:@"Blegh... %@ does NOT support AppMessages :'(", [watch name]];
            [[[UIAlertView alloc] initWithTitle:@"Connected..." message:message delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil] show];
            LOG(@"error LOG:%@", [watch friendlyDescription]);
            [self.connectedWatch closeSession:nil];
        }
    }];
}

- (void)sendAppMessages {
    if (self.isSending) {
        return;
    }
    self.isSending = YES;
    
    [self.connectedWatch appMessagesGetIsSupported:^(PBWatch *watch, BOOL isAppMessagesSupported) {
        if (isAppMessagesSupported) {
            [self.connectedWatch appMessagesLaunch:^(PBWatch *watch, NSError *error) {
                if (!error) {
                    NSArray *eventList = [self prepareEventListToSendAppMessage];
                    
                    [self.messagingQueue addObjectsFromArray:eventList];
                    self.failureCount = 0;
                    [self sendAppMessageFromQueue];

                } else {
                    LOG(@"Watch App isn't running");
                    self.isSending = NO;
                }
            }];
        } else {
            LOG(@"Watch App isn't suported");
            self.isSending = NO;
        }
    }];
}


- (NSArray*)prepareEventListToSendAppMessage {
    NSMutableArray *list = @[].mutableCopy;
    for (EKCalendar *calendar in self.selectedCalendars) {
        [list addObject:calendar];
    }
    
    NSTimeZone *timezone = [NSTimeZone systemTimeZone];
    NSInteger timezoneSec = [timezone secondsFromGMT];
    
    NSDate *startDate = [NSDate dateWithTimeIntervalSinceNow:-60*60*24];
    NSDate *endDate = [NSDate dateWithTimeIntervalSinceNow:60*60*24];
    
    NSPredicate *predicate = [self.eventStore predicateForEventsWithStartDate:startDate endDate:endDate calendars:list];
    NSArray *events = [self.eventStore eventsMatchingPredicate:predicate];
    
    NSMutableArray *updateList = [NSMutableArray array];
    
    for (EKEvent *event in events) {
        // Send data to watch:
        UIImage *titleImage = [self imegeWithEventTitle:event.title];
        PBBitmap *bitmap = [PBBitmap pebbleBitmapWithUIImage:titleImage];
        NSDictionary *update = @{EVENT_ID_KEY:[[event.eventIdentifier md5string] substringToIndex:8],
                                 EVENT_TITLE_KEY:event.title,
                                 EVENT_TITLE_IMAGE_KEY:[[bitmap pixelData] copy],
                                 EVENT_START_KEY:@((long)[event.startDate timeIntervalSince1970] + timezoneSec),
                                 EVENT_END_KEY:@((long)[event.endDate timeIntervalSince1970] + timezoneSec),
                                 };
        LOG(@"image total size:%d row size:%d", bitmap.rowSizeBytes * 8, bitmap.rowSizeBytes);
        
        [updateList addObject:update];
    }
    
    return [NSArray arrayWithArray:updateList];
}

- (UIImage*)imegeWithEventTitle:(NSString*)title {
    CGSize imageSize = CGSizeMake(96, 8.0);
    return [UIImage imageWithSize:imageSize opaque:YES scale:1.0 render:^(CGContextRef c) {
        // background
        [[UIColor blackColor] setFill];
        CGContextFillRect(c, CGRectMake(0, 0, imageSize.width, imageSize.height));
        
        // text
//        [title drawAtPoint:CGPointMake(0.0, 0.0) withAttributes:@{NSForegroundColorAttributeName: [UIColor whiteColor],
//                                                                  NSFontAttributeName: [UIFont fontWithName:@"HiraKakuProN-W3" size:16.f]}];
        [title drawAtPoint:CGPointMake(0.0, 0.0) withAttributes:@{NSForegroundColorAttributeName: [UIColor whiteColor],
                                                                  NSFontAttributeName: [UIFont fontWithName:@"MisakiGothic" size:8.f]}];
    }];
}

#pragma mark - Private Methods
- (void)startProcessing {
    if (0 < self.processingView.alpha) return;
    
    [[UIApplication sharedApplication] setNetworkActivityIndicatorVisible:YES];

    UIWindow *mainWindow = [[UIApplication sharedApplication] keyWindow];

    if (!self.processingView) {
        self.processingView = [[UIView alloc] initWithFrame:mainWindow.bounds];
        self.processingView.backgroundColor = [UIColor colorWithWhite:0.0 alpha:0.6];
    }
    
    self.processingView.alpha = 0.0;
    
    [mainWindow addSubview:self.processingView];
    [UIView animateWithDuration:0.3 animations:^{
        self.processingView.alpha = 1.0;
    }];
}
- (void)endProcessing {
    self.processingView.alpha = 1.0;
    
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(0.3 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
        [UIView animateWithDuration:0.5 animations:^{
            self.processingView.alpha = 0.0;
        } completion:^(BOOL finished) {
            [self.processingView removeFromSuperview];
            [[UIApplication sharedApplication] setNetworkActivityIndicatorVisible:NO];

        }];
    });
}

#pragma mark - PBPebbleCentral delegate methods
- (void)pebbleCentral:(PBPebbleCentral*)central watchDidConnect:(PBWatch*)watch isNew:(BOOL)isNew {
    LOG(@"%@Pebble connected:%@", [watch name], isNew ? @"New " : @"");
    self.connectedWatch = watch;
}

- (void)pebbleCentral:(PBPebbleCentral*)central watchDidDisconnect:(PBWatch*)watch {
    LOG(@"Pebble disconnected: %@", [watch name]);
    if (_connectedWatch == watch || [watch isEqual:_connectedWatch]) {
        self.connectedWatch = nil;
    }
}

#pragma mark - PBWatchDelegate
/**
 *  Called when the watch got disconnected.
 */
- (void)watchDidDisconnect:(PBWatch*)watch {
    LOG(@"watchDidDisconnect:%@", [watch name]);
}

/**
 *  Called when the watch caught an error.
 */
- (void)watch:(PBWatch*)watch handleError:(NSError*)error {
    LOG(@"watch:%@ handleError:%@", [watch name], error);
}

/**
 *  Called when the internal EASession is about to be reset.
 */
- (void)watchWillResetSession:(PBWatch*)watch {
    LOG(@"watchWillResetSession:%@", [watch name]);
}

/**
 *  Called when the internal EASession is opened
 */
- (void)watchDidOpenSession:(PBWatch*)watch {
    LOG(@"watchDidOpenSession:%@", [watch name]);
}

/**
 *  Called when the internal EASession is closed
 */
- (void)watchDidCloseSession:(PBWatch*)watch {
    LOG(@"watchDidCloseSession:%@", [watch name]);
}


#pragma mark - UITableViewDataSource
- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    return [self.calendars count];
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    NSString *identifire = [[self class] description];
    
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:identifire];
    if (!cell) {
        cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:identifire];
    }

    EKCalendar *calendar = [self.calendars objectAtIndex:indexPath.row];
    cell.textLabel.text = calendar.title;
    
    if ([self.selectedCalendars containsObject:calendar]) {
        cell.accessoryType = UITableViewCellAccessoryCheckmark;
    } else {
        cell.accessoryType = UITableViewCellAccessoryNone;
    }
    
    return cell;
}

#pragma mark - UITableViewControllerDelegate
- (void)tableView:(UITableView*)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
    EKCalendar *calendar = [self.calendars objectAtIndex:indexPath.row];
    if ([self.selectedCalendars containsObject:calendar]) {
        [self.selectedCalendars removeObject:calendar];
    } else {
        [self.selectedCalendars addObject:calendar];
    }
    [self.tableView reloadData];
}

#pragma mark - Actoins
- (IBAction)didClickSendButton:(id)sender {
    [self sendAppMessages];
}

@end
