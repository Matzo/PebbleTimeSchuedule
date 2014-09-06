//
//  UIColor+PTSFoundation.h
//  PebbleTimeSchedule
//
//  Created by Matsuo Keisuke on 2014/09/06.
//  Copyright (c) 2014年 Matsuo Keisuke. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface UIColor (PTSFoundation)
- (UIImage*)imageWithSize:(CGSize)size;
- (UIColor*)lighterColor;
- (UIColor*)darkerColor;
@end
