//
//  UIImage+PTSFoundation.h
//  PebbleTimeSchedule
//
//  Created by Matsuo Keisuke on 2014/07/20.
//  Copyright (c) 2014年 Matsuo Keisuke. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface UIImage (PTSFoundation)
+ (UIImage *)imageWithSize:(CGSize)size opaque:(BOOL)opaque scale:(CGFloat)scale render:(void (^)(CGContextRef c))render;
@end
