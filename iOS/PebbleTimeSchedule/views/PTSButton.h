#import <UIKit/UIKit.h>

@interface PTSButton : UIButton
- (UIView*)backgroundViewForState:(UIControlState)state;
- (void)setBackgroundView:(UIView*)view forStage:(UIControlState)state;
@end
