#import <UIKit/UIKit.h>

@interface UIColor (PTSFoundation)
- (UIImage*)imageWithSize:(CGSize)size;
- (UIColor*)lighterColor;
- (UIColor*)darkerColor;
@end
