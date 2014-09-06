//
//  PTSButton.m
//  PebbleTimeSchedule
//
//  Created by Matsuo Keisuke on 2014/08/10.
//  Copyright (c) 2014年 Matsuo Keisuke. All rights reserved.
//

#import "PTSButton.h"
#import "UIColor+PTSFoundation.h"

@interface PTSButton ()
@property (nonatomic, strong) NSMutableDictionary *backgroundViewDic;
@end

@implementation PTSButton

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
    self.backgroundViewDic = [NSMutableDictionary dictionary];
    
    // background color
    UIImage *backgroundImage = [self backgroundImageForState:UIControlStateNormal];
    if (!backgroundImage) backgroundImage = [[self backgroundColor] imageWithSize:CGSizeMake(1, 1)];
    UIImage *highlightedImage = [self backgroundImageForState:UIControlStateHighlighted];
    highlightedImage = (backgroundImage == highlightedImage) ? [backgroundImage darkenImage] : highlightedImage;
    
    [self setBackgroundImage:[backgroundImage resizableImage] forState:UIControlStateNormal];
    [self setBackgroundImage:[highlightedImage resizableImage] forState:UIControlStateHighlighted];
    
    // text color
    UIColor *titleColor = [self titleColorForState:UIControlStateNormal];
    UIColor *highlitedColor = [self titleColorForState:UIControlStateHighlighted];
    if (titleColor == highlitedColor) {
        [self setTitleColor:[highlitedColor darkerColor] forState:UIControlStateHighlighted];
    }
}

- (UIView*)backgroundViewForState:(UIControlState)state {
    UIView *backgroundView = [self.backgroundViewDic objectForKey:@(state)];
    return backgroundView ? backgroundView : [self.backgroundViewDic objectForKey:@(UIControlStateNormal)];
}

- (void)setBackgroundView:(UIView*)view forStage:(UIControlState)state {
    UIView *currentBackgroundView = [self.backgroundViewDic objectForKey:@(state)];
    if (currentBackgroundView) {
        [currentBackgroundView removeFromSuperview];
    }
    
    if (view) {
        [self.backgroundViewDic setObject:view forKey:@(state)];
    } else {
        [self.backgroundViewDic removeObjectForKey:@(state)];
    }
    
    [self addSubview:view];
    
    [self sendSubviewToBack:view];
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
