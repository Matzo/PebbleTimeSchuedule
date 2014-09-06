//
//  PTSButton.h
//  PebbleTimeSchedule
//
//  Created by Matsuo Keisuke on 2014/08/10.
//  Copyright (c) 2014年 Matsuo Keisuke. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface PTSButton : UIButton
- (UIView*)backgroundViewForState:(UIControlState)state;
- (void)setBackgroundView:(UIView*)view forStage:(UIControlState)state;
@end
