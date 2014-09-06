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

- (UIImage*)resizableImage {
    CGFloat top = floor(self.size.height * 0.5);
    CGFloat left = floor(self.size.width * 0.5);
    CGFloat bottom = MIN(floor(self.size.height * 0.5), top - 1.0);
    CGFloat right = MIN(floor(self.size.width * 0.5), left - 1.0);
    
    return [self resizableImageWithCapInsets:UIEdgeInsetsMake(top, left, bottom, right)];
}

- (UIImage *)darkenImage {
    CGRect rect = CGRectMake(0.0f, 0.0f, self.size.width, self.size.height);
    
    UIGraphicsBeginImageContextWithOptions(self.size, NO, 0);
    [self drawInRect:rect];
    [self drawInRect:rect blendMode:kCGBlendModeDifference alpha:0.3f];
    UIImage* blendedImage = UIGraphicsGetImageFromCurrentImageContext();
    UIGraphicsEndImageContext();
    return blendedImage;
}

@end
