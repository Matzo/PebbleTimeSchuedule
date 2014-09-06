//
//  PTSNotificationView.h
//  PebbleTimeSchedule
//
//  Created by Matsuo Keisuke on 2014/09/06.
//  Copyright (c) 2014年 Matsuo Keisuke. All rights reserved.
//

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
