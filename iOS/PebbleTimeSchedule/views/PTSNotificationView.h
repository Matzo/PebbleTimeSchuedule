#import <UIKit/UIKit.h>
#import "PTSView.h"

@interface PTSNotificationView : PTSView
@property (nonatomic, weak) IBOutlet UILabel *timeLabel;
@property (nonatomic, weak) IBOutlet UILabel *messageLabel;
- (id)initWithDate:(NSDate*)date message:(NSString*)message;
- (void)show;
- (void)dismissWithSuccess;
- (void)dismissWithFailure;
@end
