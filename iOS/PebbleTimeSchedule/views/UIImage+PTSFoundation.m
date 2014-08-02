//
//  UIImage+PTSFoundation.m
//  PebbleTimeSchedule
//
//  Created by Matsuo Keisuke on 2014/07/20.
//  Copyright (c) 2014年 Matsuo Keisuke. All rights reserved.
//

#import "UIImage+PTSFoundation.h"

@implementation UIImage (PTSFoundation)
+ (UIImage *)imageWithSize:(CGSize)size opaque:(BOOL)opaque scale:(CGFloat)scale render:(void (^)(CGContextRef c))render {
    UIGraphicsBeginImageContextWithOptions(size, opaque, scale);
    render(UIGraphicsGetCurrentContext());
    UIImage *img = UIGraphicsGetImageFromCurrentImageContext();
    UIGraphicsEndImageContext();
    return img;
}
@end
