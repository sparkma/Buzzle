影响效率的地方
1 生成spr,调试spr
2 生成layer描述文件,调试
3 layer之间跳转的代码
4 cocos经常出现zOrder不对的问题


Neighbour重构
将Bubble.Neighbour去掉

状态				Bubble处理							迁移到					Board处理
-------------		---------------------------------   ------------------		------------------------------------------------------------
[Group_NA]
BS_NA				无									BS_NAing				无
BS_NAing			无									无						无
-------------		---------------------------------   ------------------		------------------------------------------------------------
[Group_Fall]
BS_Born				播放出生动画						BS_Borning				无(出生策略函数会生成Bubble,并设置此状态)
BS_Borning			等待动画结束						BS_Fall					无
BS_Fall				经过指定的时间						BS_Falling				离开本块(注1)
					将自己的Pose设置成xxxx
BS_Falling			获取BubbleBlocker,并计算下落		BS_Drag | BS_Stop		无
-------------		---------------------------------   ------------------		------------------------------------------------------------
[Group_Drag]
BS_Drag				[显示光晕]							BS_Dragging				离开本块(注1)
					将自己的Pose设置成xxxx
BS_Dragging			无									无						无(这个在触摸事件中处理)
BS_Release			[去掉光晕]							BS_Falling 				无(触摸事件中会触发此状态)
-------------		---------------------------------   ------------------		------------------------------------------------------------
[Group_Stop]
BS_Stop				对齐坐标, 播放停止动画				BS_Stopping				无
BS_Stopping			[不]等待动画完成(注2)				BS_BlockBlend			无
-------------		---------------------------------   ------------------		------------------------------------------------------------
[Group_Blend]
BS_BlockBlend		无									BS_BlockBlending		无
BS_BlockBlending	无									BS_PoseBlend			做Block融合(注3)
BS_PoseBlend		如果正在播放停止动画,等待其完成		BS_PoseBlending			无
BS_PoseBlending		无									BS_Stand				Pose融合(注4)(因为涉及到周围Bubble,所以在Board中做)
-------------		---------------------------------   ------------------		------------------------------------------------------------
[Group_Stand]
BS_Stand			无									BS_Standing				无
BS_Standing			判断是否可以落下,如果是				BS_Fall					无
-------------		---------------------------------   ------------------		------------------------------------------------------------
[Group_Die]
BS_Die				播放死亡动画						BS_Dying				无
BS_Dying			等待动画结束						BS_Died					无
BS_Died				无									[BS_NA]					设置BS_NA,离Block,离Board数组
-------------		---------------------------------   ------------------		------------------------------------------------------------

-----------------------------------------------
注1: 为了解决一个大块被拆分成多个块的情况
当一个Bubble离开Block的时候,需要做的动作
1 对于自己,将自己的独立成一个Block,并且做动画表现融合(其实就是变成xxxx). 
2 对于其他,将其中*其他的*Bubble都拆解成单Block.
  既然这些Bubble都已经属于了一个Block, 证明这些Bubble已经处于 [Group_Blend] 或者 [Group_Stand]中
  我们需要在不修改此Bubble的Pose的情况下(为了视觉上不难受,不变成单独Bubble在融合),
  然后将Bubble状态修改成 BS_BlockBlend,,强迫重新做Block融合,以及Pose融合.
  因为此时没有正在播放停止动画,BS_PoseBlend不会等待,可以迅速完成.

-----------------------------------------------
注2:不等待动画完成是为了更早更快的使视觉上(移动位置不变了)停止的Bubble融合进入相应的Block.
前期由于等待动画完成,才进行Block融合,结果可能会使一个大的Block爆炸了,但是旁边还有一个Bubble
正在播放停止动画. 从玩家角度考虑,这个块视觉上已经停止,但是却不参与爆炸,太不合理.

-----------------------------------------------
注3: Block Blending
  检查上下左右的Bubble,查看他们的状态,如果状态在[Group_Stop][Group_Blend][Group_Stand]之中,并且颜色和自己的颜色一样
  则可以进行融合,将对方所属Block中的Bubbles全部转移到自己的Block中(不要贪便宜,将自己转移到相邻Block中).
  
  注意当一个Block正在爆炸什么的,就不能融合了.属于锁定的Block.

-----------------------------------------------
注4: Pose Blending
  取得上下左右邻居,并且是和自己一个Block的Bubble, 判断颜色,设置Bubble的新Pose.
  不是一个Block中的不予考虑,说明他们可能属于正在Falling或者别的什么状态.
  属于同一个Block的才能考虑Pose变化.


-----------------------------------------------
如果判断一个Block是否可以爆炸?
条件1: 当块中所有的Bubbles都处于BS_Standing状态
条件2: Block中Star个数大于2.
条件3: 检查Block中每个Bubble的上下左右邻居对应的Bubble.如果满足
		1 颜色和Block颜色一致
		2 状态 大于等于 BS_Falling, 小于 BS_BlockBlend
	   则需要等待,不能爆炸.
条件1 成立 并且 条件2 成立 并且 条件3不成立, 则可以爆炸.












状态			Bubble处理								迁移到					Board处理
-------------	------------------------------------------------------			--------------------
BS_NA			无										BS_NAing				无
BS_NAing		无										无						无
BS_Born			播放出生动画							BS_Borning				无(出生策略函数会生成Bubble,并设置此状态)
BS_Borning		动画结束								BS_Fall					无
BS_Fall			经过指定的时间							BS_Falling				离开本块
																				强迫Block重新自主计算未修改Pose计数
																				强迫Block重新自主计算不稳定Bubble计数
																				如果必要修改Block星星计数
BS_Falling		获取BubbleBlocker,并计算下落			BS_Drag | BS_Stop		无
BS_Drag			+1										BS_Dragging				离开本块
																				强迫Block重新自主计算未修改Pose计数
																				强迫Block重新自主计算不稳定Bubble计数
																				如果必要修改Block星星计数
BS_Dragging		无										无						无(这个在触摸事件中处理)
BS_Release		无										BS_Fall					无(触摸事件中会触发此状态)
BS_Stop			对齐坐标, 播放停止动画					BS_Stopping				进入Block
																				增加Block未修改Pose计数(_nNotBlendingCount)
																				增加不稳定Bubble计数(_nUnstableCount)
																				如果需要,增加Block中星星个数(_nStars)
																				但是不修改Bubble.Pose
BS_Stopping		等待动画完成							BS_Blend				无
BS_Blend		无										BS_Blending				无
BS_Bending		无										BS_Stand				修改Bubble的Pose
																				通知四个方向的邻居,他们的Pose已经脏了
																				减少Block中未修改Pose计数
BS_Stand		无										BS_Standing				无
BS_Standing		判断是否可以落下,如果是					BS_Fall					减少Block中不稳定Bubble计数
BS_Die			播放死亡动画							BS_Dying				无
BS_Dying		等待动画结束							BS_Died					无
BS_Died			无										[BS_NA]					设置BS_NA,离Block,离Board数组

当Board.onUpdate => _onBlockUpdate
当块中未修改Pose计数为0, 并且Block的Bubble不稳定计数为0,并且Block中Star个数大于2, 则可以Booom



Block 重构

block & bubble 释放过程
board->onUpdate ==> block->onUpdate ==> bubble->onUpdate
==> bubble -> BS_Died
此时如果释放bubble,会影响block中的bubble循环,会造成指针访问错误
此时如果释放block,会造成board中的block循环出现错误,我们已经增加了_removeBlock来防止这个

解决方法,不能在 bubble-> BS_Died中处理

08-10 14:19:53:997 - debug: bubble #01 created
08-10 14:19:53:998 - debug: block #01 created
08-10 14:19:53:999 - debug: bubble #01 leave block #-1, enter block #01
08-10 14:19:53:205 - debug: bubble #02 created
08-10 14:19:53:207 - debug: block #02 created
08-10 14:19:53:208 - debug: bubble #02 leave block #-1, enter block #02
08-10 14:19:53:405 - debug: bubble #03 created
08-10 14:19:53:408 - debug: block #03 created
08-10 14:19:53:409 - debug: bubble #03 leave block #-1, enter block #03
08-10 14:19:53:611 - debug: bubble #04 created
08-10 14:19:53:615 - debug: block #04 created
08-10 14:19:53:616 - debug: bubble #04 leave block #-1, enter block #04
08-10 14:19:54:815 - debug: bubble #05 created
08-10 14:19:54:817 - debug: block #05 created
08-10 14:19:54:818 - debug: bubble #05 leave block #-1, enter block #05
08-10 14:19:54:021 - debug: bubble #06 created
08-10 14:19:54:022 - debug: block #06 created
08-10 14:19:54:023 - debug: bubble #06 leave block #-1, enter block #06
08-10 14:19:54:235 - debug: bubble #07 created
08-10 14:19:54:237 - debug: block #07 created
08-10 14:19:54:238 - debug: bubble #07 leave block #-1, enter block #07
08-10 14:19:54:453 - debug: bubble #08 created
08-10 14:19:54:456 - debug: block #08 created
08-10 14:19:54:457 - debug: bubble #08 leave block #-1, enter block #08
08-10 14:19:54:639 - debug: bubble #01 leave block #01, enter block #03
08-10 14:19:54:643 - debug: block #01 released
08-10 14:19:55:659 - debug: bubble #09 created
08-10 14:19:55:660 - debug: block #09 created
08-10 14:19:55:661 - debug: bubble #09 leave block #-1, enter block #09
08-10 14:19:55:868 - debug: bubble #10 created
08-10 14:19:55:870 - debug: block #10 created
08-10 14:19:55:871 - debug: bubble #10 leave block #-1, enter block #10
08-10 14:19:55:277 - debug: bubble #02 leave block #02, enter block #06
08-10 14:19:55:278 - debug: block #02 released
08-10 14:19:56:826 - debug: bubble #05 leave block #05, enter block #09
08-10 14:19:56:828 - debug: block #05 released
08-10 14:19:57:593 - debug: bubble #08 (9,4) is grabbed
08-10 14:19:59:169 - debug: bubble #09 leave block #09, enter block #08
08-10 14:19:59:170 - debug: bubble #05 leave block #09, enter block #08
08-10 14:19:59:172 - debug: block #09 released
08-10 14:20:00:595 - debug: bubble #06 (10,6) is grabbed
08-10 14:20:02:688 - debug: block #11 created
08-10 14:20:02:693 - debug: bubble #06 leave block #06, enter block #-1
08-10 14:20:02:699 - debug: bubble #06 leave block #-1, enter block #11
08-10 14:20:02:273 - debug: bubble #08 leave block #08, enter block #11
08-10 14:20:02:274 - debug: bubble #09 leave block #08, enter block #11
08-10 14:20:02:277 - debug: bubble #05 leave block #08, enter block #11
08-10 14:20:02:278 - debug: block #08 released
08-10 14:20:03:531 - debug: bubble #02 (10,7) is grabbed
08-10 14:20:05:820 - debug: bubble #06 leave block #11, enter block #06
08-10 14:20:05:821 - debug: bubble #08 leave block #11, enter block #06
08-10 14:20:05:821 - debug: bubble #09 leave block #11, enter block #06
08-10 14:20:05:822 - debug: bubble #05 leave block #11, enter block #06
08-10 14:20:05:823 - debug: block #11 released
Buzzle.exe 中的 0x013b0ea2 处最可能的异常: 0xC0000005: 读取位置 0xfeeeff02 时发生访问冲突
Buzzle.exe 中的 0x013b0ea2 处未处理的异常: 0xC0000005: 读取位置 0xfeeeff02 时发生访问冲突

1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30
1  2  3  4  5  6  7  8  9 10 -- 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30
X     1
   X           2
            X           5
                     9  X
                     5
                             C
               ->            6
                     ->      8
                             9
                     X       5
               6             <-
               8
               9
               5
                             X
                                          
            

08-10 11:47:39:961 - debug: bubble #01 created
08-10 11:47:39:965 - debug: block #01 created
08-10 11:47:39:966 - debug: bubble #01 leave block #-1, enter block #01
08-10 11:47:39:213 - debug: bubble #02 created
08-10 11:47:39:220 - debug: block #02 created
08-10 11:47:39:221 - debug: bubble #02 leave block #-1, enter block #02
08-10 11:47:39:456 - debug: bubble #03 created
08-10 11:47:39:460 - debug: block #03 created
08-10 11:47:39:461 - debug: bubble #03 leave block #-1, enter block #03
08-10 11:47:40:679 - debug: bubble #04 created
08-10 11:47:40:682 - debug: block #04 created
08-10 11:47:40:682 - debug: bubble #04 leave block #-1, enter block #04
08-10 11:47:40:914 - debug: bubble #05 created
08-10 11:47:40:917 - debug: block #05 created
08-10 11:47:40:918 - debug: bubble #05 leave block #-1, enter block #05
08-10 11:47:40:148 - debug: bubble #06 created
08-10 11:47:40:151 - debug: block #06 created
08-10 11:47:40:152 - debug: bubble #06 leave block #-1, enter block #06
08-10 11:47:40:411 - debug: bubble #07 created
08-10 11:47:40:416 - debug: block #07 created
08-10 11:47:40:417 - debug: bubble #07 leave block #-1, enter block #07
08-10 11:47:41:642 - debug: bubble #08 created
08-10 11:47:41:647 - debug: block #08 created
08-10 11:47:41:649 - debug: bubble #08 leave block #-1, enter block #08
08-10 11:47:41:831 - debug: bubble #01 leave block #01, enter block #03
08-10 11:47:41:833 - debug: block #01 released
08-10 11:47:41:878 - debug: bubble #09 created
08-10 11:47:41:880 - debug: block #09 created
08-10 11:47:41:881 - debug: bubble #09 leave block #-1, enter block #09
08-10 11:47:41:102 - debug: bubble #10 created
08-10 11:47:41:103 - debug: block #10 created
08-10 11:47:41:104 - debug: bubble #10 leave block #-1, enter block #10
08-10 11:47:41:343 - debug: bubble #11 created
08-10 11:47:41:346 - debug: block #11 created
08-10 11:47:41:347 - debug: bubble #11 leave block #-1, enter block #11
08-10 11:47:42:567 - debug: bubble #02 leave block #02, enter block #06
08-10 11:47:42:570 - debug: block #02 released
08-10 11:47:42:599 - debug: bubble #12 created
08-10 11:47:42:602 - debug: block #12 created
08-10 11:47:42:603 - debug: bubble #12 leave block #-1, enter block #12
08-10 11:47:42:842 - debug: bubble #13 created
08-10 11:47:42:846 - debug: block #13 created
08-10 11:47:42:846 - debug: bubble #13 leave block #-1, enter block #13
08-10 11:47:42:076 - debug: bubble #14 created
08-10 11:47:42:079 - debug: block #14 created
08-10 11:47:42:080 - debug: bubble #14 leave block #-1, enter block #14
08-10 11:47:42:168 - debug: bubble #05 leave block #05, enter block #09
08-10 11:47:42:172 - debug: block #05 released
08-10 11:47:42:323 - debug: bubble #15 created
08-10 11:47:42:325 - debug: block #15 created
08-10 11:47:42:326 - debug: bubble #15 leave block #-1, enter block #15
08-10 11:47:43:578 - debug: bubble #16 created
08-10 11:47:43:581 - debug: block #16 created
08-10 11:47:43:583 - debug: bubble #16 leave block #-1, enter block #16
08-10 11:47:43:824 - debug: bubble #17 created
08-10 11:47:43:828 - debug: block #17 created
08-10 11:47:43:829 - debug: bubble #17 leave block #-1, enter block #17
08-10 11:47:43:869 - debug: bubble #04 leave block #04, enter block #12
08-10 11:47:43:871 - debug: block #04 released
08-10 11:47:43:050 - debug: bubble #18 created
08-10 11:47:43:053 - debug: block #18 created
08-10 11:47:43:054 - debug: bubble #18 leave block #-1, enter block #18
08-10 11:47:43:114 - debug: bubble #12 leave block #12, enter block #13
08-10 11:47:43:115 - debug: bubble #04 leave block #12, enter block #13
08-10 11:47:43:116 - debug: block #12 released
08-10 11:47:43:296 - debug: bubble #19 created
08-10 11:47:43:301 - debug: block #19 created
08-10 11:47:43:307 - debug: bubble #19 leave block #-1, enter block #19
08-10 11:47:43:394 - debug: bubble #10 leave block #10, enter block #14
08-10 11:47:43:395 - debug: block #10 released
08-10 11:47:43:515 - debug: bubble #20 created
08-10 11:47:43:523 - debug: block #20 created
08-10 11:47:43:525 - debug: bubble #20 leave block #-1, enter block #20
08-10 11:47:44:781 - debug: bubble #21 created
08-10 11:47:44:783 - debug: block #21 created
08-10 11:47:44:783 - debug: bubble #21 leave block #-1, enter block #21
08-10 11:47:44:784 - debug: bubble #15 leave block #15, enter block #16
08-10 11:47:44:785 - debug: block #15 released
08-10 11:47:44:034 - debug: bubble #22 created
08-10 11:47:44:037 - debug: block #22 created
08-10 11:47:44:038 - debug: bubble #22 leave block #-1, enter block #22
08-10 11:47:44:236 - debug: bubble #07 leave block #07, enter block #17
08-10 11:47:44:238 - debug: block #07 released
08-10 11:47:44:287 - debug: bubble #23 created
08-10 11:47:44:289 - debug: block #23 created
08-10 11:47:44:290 - debug: bubble #23 leave block #-1, enter block #23
08-10 11:47:44:543 - debug: bubble #24 created
08-10 11:47:44:544 - debug: block #24 created
08-10 11:47:44:545 - debug: bubble #24 leave block #-1, enter block #24
08-10 11:47:45:745 - debug: bubble #25 created
08-10 11:47:45:748 - debug: block #25 created
08-10 11:47:45:749 - debug: bubble #25 leave block #-1, enter block #25
08-10 11:47:45:952 - debug: bubble #26 created
08-10 11:47:45:954 - debug: block #26 created
08-10 11:47:45:956 - debug: bubble #26 leave block #-1, enter block #26
08-10 11:47:45:000 - debug: bubble #19 leave block #19, enter block #21
08-10 11:47:45:001 - debug: block #19 released
08-10 11:47:45:150 - debug: bubble #27 created
08-10 11:47:45:153 - debug: block #27 created
08-10 11:47:45:153 - debug: bubble #27 leave block #-1, enter block #27
08-10 11:47:45:363 - debug: bubble #28 created
08-10 11:47:45:366 - debug: block #28 created
08-10 11:47:45:367 - debug: bubble #28 leave block #-1, enter block #28
08-10 11:47:46:578 - debug: bubble #29 created
08-10 11:47:46:579 - debug: block #29 created
08-10 11:47:46:580 - debug: bubble #29 leave block #-1, enter block #29
08-10 11:47:46:793 - debug: bubble #30 created
08-10 11:47:46:795 - debug: block #30 created
08-10 11:47:46:795 - debug: bubble #30 leave block #-1, enter block #30
08-10 11:47:46:796 - debug: bubble #09 leave block #09, enter block #24
08-10 11:47:46:797 - debug: bubble #05 leave block #09, enter block #24
08-10 11:47:46:799 - debug: block #09 released
08-10 11:47:46:801 - debug: bubble #21 leave block #21, enter block #24
08-10 11:47:46:802 - debug: bubble #19 leave block #21, enter block #24
08-10 11:47:46:803 - debug: block #21 released
08-10 11:47:46:010 - debug: bubble #31 created
08-10 11:47:46:012 - debug: block #31 created
08-10 11:47:46:013 - debug: bubble #31 leave block #-1, enter block #31
08-10 11:47:46:075 - debug: bubble #17 leave block #17, enter block #25
08-10 11:47:46:076 - debug: bubble #07 leave block #17, enter block #25
08-10 11:47:46:077 - debug: block #17 released
08-10 11:47:46:215 - debug: bubble #32 created
08-10 11:47:46:218 - debug: block #32 created
08-10 11:47:46:218 - debug: bubble #32 leave block #-1, enter block #32
08-10 11:47:46:332 - debug: bubble #24 leave block #24, enter block #27
08-10 11:47:46:335 - debug: bubble #09 leave block #24, enter block #27
08-10 11:47:46:336 - debug: bubble #05 leave block #24, enter block #27
08-10 11:47:46:337 - debug: bubble #21 leave block #24, enter block #27
08-10 11:47:46:337 - debug: bubble #19 leave block #24, enter block #27
08-10 11:47:46:339 - debug: block #24 released
08-10 11:47:46:340 - debug: bubble #27 leave block #27, enter block #27
08-10 11:47:46:342 - debug: bubble #24 leave block #27, enter block #27
08-10 11:47:46:342 - debug: bubble #09 leave block #27, enter block #27
Buzzle.exe 中的 0x000e1106 处最可能的异常: 0xC0000005: 读取位置 0xfeeeff06 时发生访问冲突
Buzzle.exe 中的 0x000e1106 处未处理的异常: 0xC0000005: 读取位置 0xfeeeff06 时发生访问冲突

1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30
X  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30
      1
   X           2
            X           5
         X                      4
                                X  12
                                   4
                          X           10 
                                         X 15
                  X                            7   
													 X    19
				        X										    9
				                                                    5
				                                          21->      21
				                                          19->      19
				                                          X
				                               X                       17
				                                                       7
==>
1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30
X  X  3  X  X  6  X  8  X X  11 X  13 14 X  16 X  18 X  20 X  22 23 24 25 26 27 28 29 30
      1        2                   12       15                      9  17
                                   4  10                            5  7
                                                                    21
                                                                    19

08-10 11:47:46:218 - debug: bubble #32 leave block #-1, enter block #32
08-10 11:47:46:332 - debug: bubble #24 leave block #24, enter block #27
08-10 11:47:46:335 - debug: bubble #09 leave block #24, enter block #27
08-10 11:47:46:336 - debug: bubble #05 leave block #24, enter block #27
08-10 11:47:46:337 - debug: bubble #21 leave block #24, enter block #27
08-10 11:47:46:337 - debug: bubble #19 leave block #24, enter block #27
08-10 11:47:46:339 - debug: block #24 released
08-10 11:47:46:340 - debug: bubble #27 leave block #27, enter block #27
08-10 11:47:46:342 - debug: bubble #24 leave block #27, enter block #27
08-10 11:47:46:342 - debug: bubble #09 leave block #27, enter block #27
Buzzle.exe 中的 0x000e1106 处最可能的异常: 0xC0000005: 读取位置 0xfeeeff06 时发生访问冲突
Buzzle.exe 中的 0x000e1106 处未处理的异常: 0xC0000005: 读取位置 0xfeeeff06 时发生访问冲突
				                                                       
1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32
X  X  3  X  X  6  X  8  X X  11 X  13 14 X  16 X  18 X  20 X  22 23 24 25 26 27 28 29 30 31 32
      1        2                   12       15                      9  17
                                   4  10                            5  7
                                                                    21
                                                                    19
                                                                    X        24
                                                                             9
                                                                             5
                                                                             21
                                                                             19
                                                                             
HOLY !!!!
08-10 11:47:46:340 - debug: bubble #27 leave block #27, enter block #27


生成过程
概念流程
Board直接操作Bubble, BlockProp, Doodad
当生成一个 Bubble的时候, 同时生成一个Block,此时是一个Single Block
当一个Block是Single Block的时候,我们可以将Prop, Doodad attach到Single Block上

当Bubble融合的时候,我们同时处理Single Block使其融合.
	

	GUARD_FUNCTION();
	_type = bubble;
	CASprite* pspr = new BZSpriteCommon(_pgame->layer(), bubble);
	pspr->setState("na");


重大结构修改.
目前我们只生成了一套源文件BZGameLogic, 现在我们要将其分解.
同时修正一些类定义.

用以前的名词定义,那么在游戏中无论是单个的Block,还是融合了的很多Block其实都是Block, 融合了的是一个大的Block而已. 而不应该叫Group. 基于以前的定义为了能有效处理Group的行为,我们需要生成每个Block的时候,都为其生成一个Group, 当有Block融合的时候,我们要合并Group.

上述的名称为Group的东西和单个的Block(其实他也一定有个Group), 一方面是可以看成两种东西,一方面又可以看成是相同的东西,都是Group. 这样概念不清晰, Group已经不能正确表达游戏中的意义,

所以决定做一下的修改. 以前的Block概念将不再存在. 新的Block可以是一个单独的泡泡,也可以是融合了的大泡泡.

a) 首先我们需要有个Game, 为了后续生成 GameClassic(经典模式), GameSurvival(极限生存模式), GameFunny(随便玩玩模式)等等. 
b) 然后我们有个Board(棋盘,板)概念, 他在Game中. 
c) 在Board中我们会生成Block, 相同类型的Block可以融合在一起,还是叫Block. 
d) Block中有一个(单个的Block)或者多个(如果Block是大的,是融合的)Bubble, 就是我们以前的Block. 现在叫Bubble.
e) Block中有Prop(道具), 可以派生出PropStar, PropBomb等.
f) Block中可以有Doodad(装饰,也可以是Particles), 这个目前可以没有,后续会有很多, 比如Block融合,消亡等事件发生的时候, 我们可以生成一些Doodads, 去壮观效果. 这些东西不会对Block属性起任何作用,只是为了显示效果.

牛超看看这个,不明白的和我讨论.
