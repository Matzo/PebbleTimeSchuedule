//
//  PTSCalendarListViewController.m
//  PebbleTimeSchedule
//
//  Created by Matsuo Keisuke on 2014/07/01.
//  Copyright (c) 2014年 Matsuo Keisuke. All rights reserved.
//

#import "PTSCalendarListViewController.h"
#import <PebbleKit/PebbleKit.h>
#import <EventKit/EventKit.h>
#import "UIImage+PTSFoundation.h"

@interface PTSCalendarListViewController()<PBPebbleCentralDelegate>
@property (nonatomic, strong) EKEventStore *eventStore;
@property (nonatomic, strong) NSArray *calendars;
@property (nonatomic, strong) NSMutableSet *selectedCalendars;
@property (nonatomic, strong) PBWatch *targetWatch;
@property (nonatomic, strong) NSMutableArray *messagingQueue;
@property (nonatomic, assign) BOOL isSending;
@property (nonatomic, assign) NSInteger failureCount;
@end


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

    
    UIImage *testImage = [self imegeWithEventTitle:@"テストイベント"];
    [PBBitmap pebbleBitmapWithUIImage:testImage];

}

- (void)initialize {
    self.title = @"Pebble TimeSchedule";
    self.navigationItem.prompt = @"Select calendars to send to Pebble";
    self.selectedCalendars = [[NSMutableSet alloc] init];
    self.messagingQueue = [NSMutableArray array];
    self.failureCount = 0;
    
    [[PBPebbleCentral defaultCentral] setDelegate:self];
    [[PBPebbleCentral defaultCentral] setAppUUID:[self createUUID]];
    [self setTargetWatch:[[PBPebbleCentral defaultCentral] lastConnectedWatch]];
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

#pragma mark - Public Methods

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

- (void)sendAppMessageQueue {
    if (self.messagingQueue.count == 0 || 20 < self.failureCount) {
        [self.targetWatch closeSession:nil];
        return;
    }
    
    [self.targetWatch appMessagesGetIsSupported:^(PBWatch *watch, BOOL isAppMessagesSupported) {
        if (isAppMessagesSupported) {
            NSDictionary *firstEvent = self.messagingQueue.firstObject;
            NSLog(@"[send AppMessage]:%@", firstEvent);

            [self.targetWatch appMessagesPushUpdate:firstEvent onSent:^(PBWatch *watch, NSDictionary *update, NSError *error) {
                float nextDuration;
                if (!error) {
                    self.isSending = NO;
                    // success
                    NSLog(@"[Success!]send AppMessage:%@", update);
                    [self.messagingQueue removeObject:firstEvent];
                    nextDuration = 0.25;
                } else {
                    // failure
                    self.failureCount++;
                    NSLog(@"[Failure!]send AppMessage error:%@", error);
                    nextDuration = 0.5;
                }
                dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(nextDuration * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
                    [self sendAppMessageQueue];
                });
            }];
        } else {
            self.isSending = NO;
            
            NSString *message = [NSString stringWithFormat:@"Blegh... %@ does NOT support AppMessages :'(", [watch name]];
            [[[UIAlertView alloc] initWithTitle:@"Connected..." message:message delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil] show];
            NSLog(@"error LOG:%@", [watch friendlyDescription]);
            [self.targetWatch closeSession:nil];
        }
    }];

}

- (void)sendAppMessage:(NSArray*)updateList {
    if (self.isSending) {
        return;
    }
    self.isSending = YES;

    [self.targetWatch appMessagesGetIsSupported:^(PBWatch *watch, BOOL isAppMessagesSupported) {
        if (isAppMessagesSupported) {
            [self.targetWatch appMessagesLaunch:^(PBWatch *watch, NSError *error) {
                if (!error) {
                    [self.messagingQueue addObjectsFromArray:updateList];
                    self.failureCount = 0;
                    [self sendAppMessageQueue];

                } else {
                    self.isSending = NO;
                }
            }];
        } else {
            self.isSending = NO;
        }
    }];
}

- (void)setTargetWatch:(PBWatch*)watch {
    _targetWatch = watch;
}

- (void)sendEventListToPebble {
    NSMutableArray *list = @[].mutableCopy;
    for (EKCalendar *calendar in self.selectedCalendars) {
        [list addObject:calendar];
    }
    
    NSTimeZone *timezone = [NSTimeZone systemTimeZone];
    NSInteger timezoneSec = [timezone secondsFromGMT];
//    NSTimeInterval timeinterval = [[NSDate date] timeIntervalSince1970] + timezoneSec;
//    NSTimeInterval timeinterval = [[NSDate date] timeIntervalSince1970] + timezoneSec;
//    NSTimeInterval timeIntervalOfToday = timeinterval - ((long)timeinterval % (60*60*24));
//    NSDate *today = [NSDate dateWithTimeIntervalSince1970:timeIntervalOfToday];
//    NSDate *tomorrow = [today dateByAddingTimeInterval:60*60*24*2];
    
    NSDate *startDate = [NSDate dateWithTimeIntervalSinceNow:-60*60*24];
    NSDate *endDate = [NSDate dateWithTimeIntervalSinceNow:60*60*24];
    
    NSPredicate *predicate = [self.eventStore predicateForEventsWithStartDate:startDate endDate:endDate calendars:list];
    NSArray *events = [self.eventStore eventsMatchingPredicate:predicate];
    
//    NSLog(@"timzone sec:%d normal sec:%f", [timezone secondsFromGMT], timeinterval);
    NSLog(@"event list:%@", events);
    
    NSMutableArray *updateList = [NSMutableArray array];
    
    for (EKEvent *event in events) {
        // Send data to watch:
        // See demos/feature_app_messages/weather.c in the native watch app SDK for the same definitions on the watch's end:
        NSNumber *idKey = @(1 << 0);
        NSNumber *titleKey = @(1 << 1);
        NSNumber *titleImageKey = @(1 << 2);
        NSNumber *startKey = @(1 << 3);
        NSNumber *endKey = @(1 << 4);
        UIImage *titleImage = [self imegeWithEventTitle:event.title];
        PBBitmap *bitmap = [PBBitmap pebbleBitmapWithUIImage:titleImage];
        NSLog(@"bitmap row size:%d info flag:%d actual size:%d pixel:%@", bitmap.rowSizeBytes, bitmap.infoFlags, [bitmap.pixelData length], [bitmap pixelData]);
        
        NSDictionary *update = @{ idKey:event.eventIdentifier,
                                  titleKey:[NSString stringWithFormat:@"event %d", [events indexOfObject:event] + 1],
                                  titleImageKey:[[bitmap pixelData] copy],
                                  startKey:@((long)[event.startDate timeIntervalSince1970] + timezoneSec),
                                  endKey:@((long)[event.endDate timeIntervalSince1970] + timezoneSec)};
        NSLog(@"event name:%@ event:%@", event.title, event);
        
        [updateList addObject:update];
        
        [self sendAppMessage:updateList];
    }
}

- (UIImage*)imegeWithEventTitle:(NSString*)title {
    CGSize imageSize = CGSizeMake(100.0, 16.0);
    return [UIImage imageWithSize:imageSize opaque:YES scale:1.0 render:^(CGContextRef c) {
        // background
        [[UIColor blackColor] setFill];
        CGContextFillRect(c, CGRectMake(0, 0, imageSize.width, imageSize.height));
        
        // text
        [title drawAtPoint:CGPointMake(0.0, 0.0) withAttributes:@{NSForegroundColorAttributeName: [UIColor whiteColor],
                                                                  NSFontAttributeName: [UIFont fontWithName:@"HiraKakuProN-W3" size:16.f]}];
//        [title drawAtPoint:CGPointMake(0.0, 0.0) withAttributes:@{NSForegroundColorAttributeName: [UIColor whiteColor],
//                                                                  NSFontAttributeName: [UIFont fontWithName:@"MisakiGothic" size:8.f]}];
    }];
}

#pragma mark - PBPebbleCentral delegate methods
- (void)pebbleCentral:(PBPebbleCentral*)central watchDidConnect:(PBWatch*)watch isNew:(BOOL)isNew {
    [self setTargetWatch:watch];
}

- (void)pebbleCentral:(PBPebbleCentral*)central watchDidDisconnect:(PBWatch*)watch {
//    [[[UIAlertView alloc] initWithTitle:@"Disconnected!" message:[watch name] delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil] show];
    if (_targetWatch == watch || [watch isEqual:_targetWatch]) {
        [self setTargetWatch:nil];
    }
}

#pragma mark - UITableViewDataSource
- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    return [self.calendars count];
}

// Row display. Implementers should *always* try to reuse cells by setting each cell's reuseIdentifier and querying for available reusable cells with dequeueReusableCellWithIdentifier:
// Cell gets various attributes set automatically based on table (separators) and data source (accessory views, editing controls)

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
    [self sendEventListToPebble];
}

@end
