//
//  PTSView.m
//  PebbleTimeSchedule
//
//  Created by Matsuo Keisuke on 2014/09/06.
//  Copyright (c) 2014年 Matsuo Keisuke. All rights reserved.
//

#import "PTSView.h"

@implementation PTSView

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        // Initialization code
        [self initialize];
    }
    return self;
}

- (void)awakeFromNib {
    [self initialize];
}

- (void)initialize {
    UINib *nib = [UINib nibWithNibName:NSStringFromClass([self class]) bundle:[NSBundle bundleForClass:([self class])]];
    NSArray *views = [nib instantiateWithOwner:self options:nil];
    UIView *firstView = [views firstObject];
    firstView.frame = self.frame;
    firstView.autoresizingMask = (UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight);
    self.backgroundColor = firstView.backgroundColor;
    
    for (UIView *view in firstView.subviews) {
        [self addSubview:view];
    }
}
/*
// Only override drawRect: if you perform custom drawing.
// An empty implementation adversely affects performance during animation.
- (void)drawRect:(CGRect)rect
{
    // Drawing code
}
*/

@end
