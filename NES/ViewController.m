//
//  ViewController.m
//
//  Created by richardwallis on 15/11/2022.
//

#import "ViewController.h"
#import "EmulationController.h"

@interface ViewController ()

@property (nonatomic,readwrite) EmulationController*        systemController;
@property (nonatomic,readwrite) EmulationMetalView*         metalView;

@end

@implementation ViewController

- (void)viewDidLoad
{
    [super viewDidLoad];

    self.metalView = [EmulationMetalView new];
    self.systemController = [[EmulationController alloc] initWithView:self.metalView];
    self.metalView.delegate = self.systemController;
    
    [self.view addSubview:self.metalView];
}

- (void) viewWillAppear
{
    self.metalView.frame = self.view.frame;
}

- (void) viewWillLayout
{
    self.metalView.frame = self.view.frame;
}

@end
