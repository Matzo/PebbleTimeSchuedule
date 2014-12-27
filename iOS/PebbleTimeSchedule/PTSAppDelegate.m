#import "PTSAppDelegate.h"
#import "PTSCalendarListViewController.h"
#import <Parse/Parse.h>
#import "PTSConfigurationManager.h"

@interface PTSAppDelegate()
@property (nonatomic, assign) UIBackgroundTaskIdentifier bgTask;
@property (nonatomic, strong) PTSCalendarListViewController *calendarListViewController;
@end

@implementation PTSAppDelegate

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    [self setupExternalLibrary];

    self.window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
    // Override point for customization after application launch.
    self.window.backgroundColor = [UIColor whiteColor];
    [self.window makeKeyAndVisible];
    
    PTSCalendarListViewController *calendarListViewController = [[PTSCalendarListViewController alloc] initWithNibName:NSStringFromClass([PTSCalendarListViewController class]) bundle:nil];
    self.calendarListViewController = calendarListViewController;
    
    UINavigationController *navi = [[UINavigationController alloc] initWithRootViewController:calendarListViewController];
//    [navi.navigationBar setBarStyle:UIBarStyleDefault];
    
    self.window.rootViewController = navi;
    
    // Register for Push Notitications
    UIUserNotificationType userNotificationTypes = (UIUserNotificationTypeAlert |
                                                    UIUserNotificationTypeBadge |
                                                    UIUserNotificationTypeSound);
    UIUserNotificationSettings *settings = [UIUserNotificationSettings settingsForTypes:userNotificationTypes
                                                                             categories:nil];
    [application registerUserNotificationSettings:settings];
    [application registerForRemoteNotifications];
    
    return YES;
}

- (void)applicationWillResignActive:(UIApplication *)application
{
    // Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
    // Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
}

- (void)applicationDidEnterBackground:(UIApplication *)application
{
    // Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later. 
    // If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
    
    self.bgTask = [application beginBackgroundTaskWithName:@"RefreshTask" expirationHandler:^{
        // Clean up any unfinished task business by marking where you
        // stopped or ending the task outright.
        [application endBackgroundTask:self.bgTask];
        self.bgTask = UIBackgroundTaskInvalid;
    }];
    
    // Start the long-running task and return immediately.
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        
        // Do the work associated with the task, preferably in chunks.
        NSLog(@"reflesh pebble data");
        
        
        [application endBackgroundTask:self.bgTask];
        self.bgTask = UIBackgroundTaskInvalid;
    });
}

- (void)applicationWillEnterForeground:(UIApplication *)application
{
    // Called as part of the transition from the background to the inactive state; here you can undo many of the changes made on entering the background.
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
    // Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
}

- (void)applicationWillTerminate:(UIApplication *)application
{
    // Called when the application is about to terminate. Save data if appropriate. See also applicationDidEnterBackground:.
}


#pragma mark - PushNotification
- (void)application:(UIApplication *)application didRegisterUserNotificationSettings:(UIUserNotificationSettings *)notificationSettings {
}

- (void)application:(UIApplication *)application didFailToRegisterForRemoteNotificationsWithError:(NSError *)error {
    if ([error code] == 3010) {
        LOG(@"Push notifications don't work in the simulator!");
    } else {
        LOG(@"didFailToRegisterForRemoteNotificationsWithError: %@", error);
    }
}

- (void)application:(UIApplication *)application didRegisterForRemoteNotificationsWithDeviceToken:(NSData *)deviceToken {
    // Store the deviceToken in the current installation and save it to Parse.
    PFInstallation *currentInstallation = [PFInstallation currentInstallation];
    [currentInstallation setDeviceTokenFromData:deviceToken];
    currentInstallation.channels = @[ @"global" ];
    [currentInstallation saveInBackground];
}

//- (void)application:(UIApplication *)application didReceiveRemoteNotification:(NSDictionary *)userInfo {
//    
////    [PFPush handlePush:userInfo];
//}
- (void)application:(UIApplication *)application didReceiveRemoteNotification:(NSDictionary *)userInfo fetchCompletionHandler:(void (^)(UIBackgroundFetchResult))completionHandler {
    NSLog(@"reflesh pebble data");

    [self.calendarListViewController sendAppMessagesWithCompletion:^{
        completionHandler(UIBackgroundFetchResultNoData);
    }];
    
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(5.0 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
        completionHandler(UIBackgroundFetchResultNoData);
    });
}
#pragma mark - Custom Setup
- (void)setupExternalLibrary {
    // setup for Crashlytics
    [Fabric with:@[CrashlyticsKit]];

    // setup for Parse
    PTSConfigurationManager *conf = [PTSConfigurationManager sharedObjectWithBundle:nil];
    if ([conf configurationForKey:kConfigurationParseApplicationId] && [conf configurationForKey:kConfigurationParseClientKey]) {
        [Parse setApplicationId:[conf configurationForKey:kConfigurationParseApplicationId]
                      clientKey:[conf configurationForKey:kConfigurationParseClientKey]];
    }
}

@end
