#import <UIKit/UIKit.h>

@interface PTSCalendarListViewController : UIViewController<
UITableViewDataSource,
UITableViewDelegate
>
@property (nonatomic, weak) IBOutlet UITableView *tableView;
@property (nonatomic, weak) IBOutlet UIButton *sendButton;

- (void)sendAppMessagesWithCompletion:(void(^)(void))fetchedBlock;
- (void)sendDeviceTokenWithCompletion:(void(^)(void))fetchedBlock;

@end
