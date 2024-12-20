declare i32 @getint()
declare i32 @getch()
declare float @getfloat()
declare i32 @getarray(ptr)
declare i32 @getfarray(ptr)
declare void @putint(i32)
declare void @putch(i32)
declare void @putfloat(float)
declare void @putarray(i32,ptr)
declare void @putfarray(i32,ptr)
declare void @_sysy_starttime(i32)
declare void @_sysy_stoptime(i32)
declare void @llvm.memset.p0.i32(ptr,i8,i32,i1)
declare i32 @llvm.umax.i32(i32,i32)
declare i32 @llvm.umin.i32(i32,i32)
declare i32 @llvm.smax.i32(i32,i32)
declare i32 @llvm.smin.i32(i32,i32)
@UP = global i32 0
@DOWN = global i32 1
@LEFT = global i32 2
@RIGHT = global i32 3
@MAP_LEN = global i32 4
@POW2 = global [20 x i32] [i32 1,i32 2,i32 4,i32 8,i32 16,i32 32,i32 64,i32 128,i32 256,i32 512,i32 1024,i32 2048,i32 4096,i32 8192,i32 16384,i32 32768,i32 65536,i32 131072,i32 262144,i32 524288]
@LEN_OF_POW2 = global [20 x i32] [i32 0,i32 1,i32 1,i32 1,i32 2,i32 2,i32 2,i32 3,i32 3,i32 3,i32 4,i32 4,i32 4,i32 4,i32 5,i32 5,i32 5,i32 6,i32 6,i32 6]
@STR_INIT = global [25 x i32] [i32 73,i32 110,i32 112,i32 117,i32 116,i32 32,i32 97,i32 32,i32 114,i32 97,i32 110,i32 100,i32 111,i32 109,i32 32,i32 110,i32 117,i32 109,i32 98,i32 101,i32 114,i32 58,i32 32,i32 10,i32 0]
@STR_HELP = global [62 x i32] [i32 119,i32 44,i32 32,i32 97,i32 44,i32 32,i32 115,i32 44,i32 32,i32 100,i32 58,i32 32,i32 109,i32 111,i32 118,i32 101,i32 10,i32 104,i32 58,i32 32,i32 112,i32 114,i32 105,i32 110,i32 116,i32 32,i32 116,i32 104,i32 105,i32 115,i32 32,i32 104,i32 101,i32 108,i32 112,i32 10,i32 113,i32 58,i32 32,i32 113,i32 117,i32 105,i32 116,i32 10,i32 112,i32 58,i32 32,i32 112,i32 114,i32 105,i32 110,i32 116,i32 32,i32 116,i32 104,i32 101,i32 32,i32 109,i32 97,i32 112,i32 10,i32 0]
@STR_SCORE = global [8 x i32] [i32 115,i32 99,i32 111,i32 114,i32 101,i32 58,i32 32,i32 0]
@STR_STEP = global [7 x i32] [i32 115,i32 116,i32 101,i32 112,i32 58,i32 32,i32 0]
@STR_GG = global [12 x i32] [i32 71,i32 97,i32 109,i32 101,i32 32,i32 111,i32 118,i32 101,i32 114,i32 46,i32 10,i32 0]
@STR_INVALID = global [26 x i32] [i32 73,i32 110,i32 118,i32 97,i32 108,i32 105,i32 100,i32 32,i32 105,i32 110,i32 112,i32 117,i32 116,i32 46,i32 32,i32 84,i32 114,i32 121,i32 32,i32 97,i32 103,i32 97,i32 105,i32 110,i32 46,i32 0]
@CHR_SPACE = global i32 32
@CHR_ENTER = global i32 10
@map = global [4x [4x i32]] zeroinitializer
@score = global i32 zeroinitializer
@step = global i32 zeroinitializer
@max_num_len = global i32 zeroinitializer
@alive = global i32 zeroinitializer
@seed = global i32 zeroinitializer
define i32 @or(i32 %r0,i32 %r1)
{
L0:  ;
    br label %L1
L1:  ;
    %r5 = icmp ne i32 %r0,0
    br i1 %r5, label %L2, label %L4
    %r7 = icmp ne i32 %r1,0
    ret i32 0
L2:  ;
    %r8 = add i32 1,0
    ret i32 %r8
L3:  ;
    %r9 = add i32 0,0
    ret i32 %r9
L4:  ;
    br i1 %r7, label %L2, label %L3
}
define void @put_string(ptr %r0)
{
L0:  ;
    br label %L1
L1:  ;
    %r2 = add i32 0,0
    br label %L2
L2:  ;
    %r14 = phi i32 [%r2,%L1],[%r13,%L3]
    %r4 = getelementptr i32, ptr %r0, i32 %r14
    %r5 = load i32, ptr %r4
    %r6 = add i32 0,0
    %r7 = icmp ne i32 %r5,%r6
    br i1 %r7, label %L3, label %L4
L3:  ;
    %r9 = getelementptr i32, ptr %r0, i32 %r14
    %r10 = load i32, ptr %r9
    call void @putch(i32 %r10)
    %r12 = add i32 1,0
    %r13 = add i32 %r14,%r12
    br label %L2
L4:  ;
    ret void
}
define i32 @rand()
{
L0:  ;
    br label %L1
L1:  ;
    %r0 = load i32, ptr @seed
    %r1 = add i32 214013,0
    %r2 = mul i32 %r0,%r1
    %r3 = add i32 2531011,0
    %r4 = add i32 %r2,%r3
    %r5 = add i32 1073741824,0
    %r6 = srem i32 %r4,%r5
    store i32 %r6, ptr @seed
    %r7 = load i32, ptr @seed
    %r8 = add i32 0,0
    %r9 = icmp slt i32 %r7,%r8
    br i1 %r9, label %L2, label %L3
L2:  ;
    %r10 = load i32, ptr @seed
    %r11 = sub i32 0,%r10
    store i32 %r11, ptr @seed
    br label %L3
L3:  ;
    %r12 = load i32, ptr @seed
    %r13 = add i32 65536,0
    %r14 = sdiv i32 %r12,%r13
    %r15 = add i32 32768,0
    %r16 = srem i32 %r14,%r15
    ret i32 %r16
}
define void @clear_map()
{
L0:  ;
    br label %L1
L1:  ;
    %r1 = add i32 0,0
    %r3 = add i32 0,0
    br label %L2
L2:  ;
    %r23 = phi i32 [%r1,%L1],[%r20,%L7]
    %r21 = phi i32 [%r3,%L1],[%r22,%L7]
    %r5 = load i32, ptr @MAP_LEN
    %r6 = icmp slt i32 %r23,%r5
    br i1 %r6, label %L3, label %L4
L3:  ;
    %r7 = add i32 0,0
    br label %L5
L4:  ;
    ret void
L5:  ;
    %r22 = phi i32 [%r7,%L3],[%r17,%L6]
    %r9 = load i32, ptr @MAP_LEN
    %r10 = icmp slt i32 %r22,%r9
    br i1 %r10, label %L6, label %L7
L6:  ;
    %r13 = getelementptr [4 x [4 x i32]], ptr @map, i32 0, i32 %r23, i32 %r22
    %r14 = add i32 0,0
    store i32 %r14, ptr %r13
    %r16 = add i32 1,0
    %r17 = add i32 %r22,%r16
    br label %L5
L7:  ;
    %r19 = add i32 1,0
    %r20 = add i32 %r23,%r19
    br label %L2
}
define void @init()
{
L0:  ;
    br label %L1
L1:  ;
    call void @clear_map()
    %r0 = add i32 0,0
    store i32 %r0, ptr @score
    %r1 = add i32 0,0
    store i32 %r1, ptr @step
    %r2 = add i32 1,0
    store i32 %r2, ptr @max_num_len
    %r3 = add i32 1,0
    store i32 %r3, ptr @alive
    ret void
}
define void @print_map()
{
L0:  ;
    br label %L1
L1:  ;
    %r0 = load i32, ptr @CHR_ENTER
    call void @putch(i32 %r0)
    %r1 = getelementptr [7 x i32], ptr @STR_STEP, i32 0
    call void @put_string(ptr %r1)
    %r2 = load i32, ptr @step
    call void @putint(i32 %r2)
    %r3 = load i32, ptr @CHR_ENTER
    call void @putch(i32 %r3)
    %r4 = getelementptr [8 x i32], ptr @STR_SCORE, i32 0
    call void @put_string(ptr %r4)
    %r5 = load i32, ptr @score
    call void @putint(i32 %r5)
    %r6 = load i32, ptr @CHR_ENTER
    call void @putch(i32 %r6)
    %r8 = add i32 0,0
    %r10 = add i32 0,0
    br label %L2
L2:  ;
    %r78 = phi i32 [%r8,%L1],[%r67,%L7]
    %r76 = phi i32 [%r10,%L1],[%r77,%L7]
    %r12 = load i32, ptr @MAP_LEN
    %r13 = icmp slt i32 %r78,%r12
    br i1 %r13, label %L3, label %L4
L3:  ;
    %r14 = add i32 0,0
    br label %L5
L4:  ;
    ret void
L5:  ;
    %r77 = phi i32 [%r14,%L3],[%r63,%L10]
    %r16 = load i32, ptr @MAP_LEN
    %r17 = icmp slt i32 %r77,%r16
    br i1 %r17, label %L6, label %L7
L6:  ;
    %r20 = getelementptr [4 x [4 x i32]], ptr @map, i32 0, i32 %r78, i32 %r77
    %r21 = load i32, ptr %r20
    %r22 = add i32 0,0
    %r23 = icmp eq i32 %r21,%r22
    br i1 %r23, label %L8, label %L9
L7:  ;
    %r64 = load i32, ptr @CHR_ENTER
    call void @putch(i32 %r64)
    %r66 = add i32 1,0
    %r67 = add i32 %r78,%r66
    br label %L2
L8:  ;
    %r27 = getelementptr [4 x [4 x i32]], ptr @map, i32 0, i32 %r78, i32 %r77
    %r28 = load i32, ptr %r27
    %r29 = getelementptr [20 x i32], ptr @LEN_OF_POW2, i32 0, i32 %r28
    %r30 = load i32, ptr %r29
    %r31 = add i32 1,0
    %r32 = add i32 %r30,%r31
    br label %L11
L9:  ;
    %r43 = getelementptr [4 x [4 x i32]], ptr @map, i32 0, i32 %r78, i32 %r77
    %r44 = load i32, ptr %r43
    %r45 = getelementptr [20 x i32], ptr @POW2, i32 0, i32 %r44
    %r46 = load i32, ptr %r45
    call void @putint(i32 %r46)
    %r50 = getelementptr [4 x [4 x i32]], ptr @map, i32 0, i32 %r78, i32 %r77
    %r51 = load i32, ptr %r50
    %r52 = getelementptr [20 x i32], ptr @LEN_OF_POW2, i32 0, i32 %r51
    %r53 = load i32, ptr %r52
    br label %L14
L10:  ;
    %r62 = add i32 1,0
    %r63 = add i32 %r77,%r62
    br label %L5
L11:  ;
    %r75 = phi i32 [%r32,%L8],[%r39,%L12]
    %r34 = load i32, ptr @max_num_len
    %r35 = icmp sle i32 %r75,%r34
    br i1 %r35, label %L12, label %L13
L12:  ;
    %r36 = add i32 95,0
    call void @putch(i32 %r36)
    %r38 = add i32 1,0
    %r39 = add i32 %r75,%r38
    br label %L11
L13:  ;
    %r40 = load i32, ptr @CHR_SPACE
    call void @putch(i32 %r40)
    br label %L10
L14:  ;
    %r71 = phi i32 [%r53,%L9],[%r60,%L15]
    %r55 = load i32, ptr @max_num_len
    %r56 = icmp sle i32 %r71,%r55
    br i1 %r56, label %L15, label %L16
L15:  ;
    %r57 = load i32, ptr @CHR_SPACE
    call void @putch(i32 %r57)
    %r59 = add i32 1,0
    %r60 = add i32 %r71,%r59
    br label %L14
L16:  ;
    br label %L10
}
define i32 @move_base(i32 %r0,ptr %r1,ptr %r2,ptr %r3,ptr %r4,i32 %r5)
{
L0:  ;
    br label %L1
L1:  ;
    %r9 = add i32 0,0
    %r11 = add i32 0,0
    %r13 = add i32 0,0
    %r15 = add i32 1,0
    %r16 = sub i32 0,%r15
    %r17 = icmp eq i32 %r0,%r16
    br i1 %r17, label %L2, label %L3
L2:  ;
    %r18 = load i32, ptr @MAP_LEN
    %r19 = add i32 1,0
    %r20 = sub i32 %r18,%r19
    %r21 = add i32 1,0
    %r22 = sub i32 0,%r21
    br label %L4
L3:  ;
    %r23 = add i32 0,0
    %r24 = load i32, ptr @MAP_LEN
    br label %L4
L4:  ;
    %r192 = phi i32 [%r20,%L2],[%r23,%L3]
    %r191 = phi i32 [%r22,%L2],[%r24,%L3]
    %r25 = add i32 0,0
    %r26 = getelementptr i32, ptr %r2, i32 %r25
    store i32 %r192, ptr %r26
    br label %L5
L5:  ;
    %r189 = phi i32 [%r13,%L4],[%r188,%L10]
    %r28 = add i32 0,0
    %r29 = getelementptr i32, ptr %r2, i32 %r28
    %r30 = load i32, ptr %r29
    %r32 = icmp ne i32 %r30,%r191
    br i1 %r32, label %L6, label %L7
L6:  ;
    %r36 = add i32 %r192,%r0
    br label %L8
L7:  ;
    ret i32 %r189
L8:  ;
    %r188 = phi i32 [%r189,%L6],[%r188,%L11],[%r187,%L15]
    %r181 = phi i32 [%r36,%L6],[%r47,%L11],[%r183,%L15]
    %r178 = phi i32 [%r192,%L6],[%r178,%L11],[%r177,%L15]
    %r41 = icmp ne i32 %r181,%r191
    br i1 %r41, label %L9, label %L10
L9:  ;
    %r44 = icmp eq i32 %r181,%r178
    br i1 %r44, label %L11, label %L12
L10:  ;
    %r167 = add i32 0,0
    %r168 = getelementptr i32, ptr %r2, i32 %r167
    %r169 = add i32 0,0
    %r170 = getelementptr i32, ptr %r2, i32 %r169
    %r171 = load i32, ptr %r170
    %r173 = add i32 %r171,%r0
    store i32 %r173, ptr %r168
    br label %L5
L11:  ;
    %r47 = add i32 %r181,%r0
    br label %L8
L12:  ;
    %r48 = add i32 0,0
    %r49 = getelementptr i32, ptr %r1, i32 %r48
    store i32 %r181, ptr %r49
    %r52 = add i32 0,0
    %r53 = getelementptr i32, ptr %r3, i32 %r52
    %r54 = load i32, ptr %r53
    %r55 = add i32 0,0
    %r56 = getelementptr i32, ptr %r4, i32 %r55
    %r57 = load i32, ptr %r56
    %r58 = getelementptr [4 x [4 x i32]], ptr @map, i32 0, i32 %r54, i32 %r57
    %r59 = load i32, ptr %r58
    %r60 = add i32 0,0
    %r61 = getelementptr i32, ptr %r1, i32 %r60
    store i32 %r178, ptr %r61
    %r64 = add i32 0,0
    %r65 = getelementptr i32, ptr %r3, i32 %r64
    %r66 = load i32, ptr %r65
    %r67 = add i32 0,0
    %r68 = getelementptr i32, ptr %r4, i32 %r67
    %r69 = load i32, ptr %r68
    %r70 = getelementptr [4 x [4 x i32]], ptr @map, i32 0, i32 %r66, i32 %r69
    %r71 = load i32, ptr %r70
    %r73 = add i32 0,0
    %r74 = icmp eq i32 %r71,%r73
    br i1 %r74, label %L13, label %L14
L13:  ;
    %r76 = add i32 0,0
    %r77 = icmp eq i32 %r59,%r76
    br i1 %r77, label %L16, label %L17
L14:  ;
    %r109 = icmp eq i32 %r59,%r71
    br i1 %r109, label %L21, label %L22
L15:  ;
    %r187 = phi i32 [%r186,%L18],[%r190,%L23]
    %r183 = phi i32 [%r182,%L18],[%r185,%L23]
    %r177 = phi i32 [%r178,%L18],[%r176,%L23]
    br label %L8
L16:  ;
    %r80 = add i32 %r181,%r0
    br label %L18
L17:  ;
    %r82 = icmp ne i32 %r5,0
    br i1 %r82, label %L19, label %L20
L18:  ;
    %r186 = phi i32 [%r188,%L16],[%r103,%L20]
    %r182 = phi i32 [%r80,%L16],[%r106,%L20]
    br label %L15
L19:  ;
    %r83 = add i32 1,0
    ret i32 %r83
L20:  ;
    %r84 = add i32 0,0
    %r85 = getelementptr i32, ptr %r3, i32 %r84
    %r86 = load i32, ptr %r85
    %r87 = add i32 0,0
    %r88 = getelementptr i32, ptr %r4, i32 %r87
    %r89 = load i32, ptr %r88
    %r90 = getelementptr [4 x [4 x i32]], ptr @map, i32 0, i32 %r86, i32 %r89
    store i32 %r59, ptr %r90
    %r92 = add i32 0,0
    %r93 = getelementptr i32, ptr %r1, i32 %r92
    store i32 %r181, ptr %r93
    %r95 = add i32 0,0
    %r96 = getelementptr i32, ptr %r3, i32 %r95
    %r97 = load i32, ptr %r96
    %r98 = add i32 0,0
    %r99 = getelementptr i32, ptr %r4, i32 %r98
    %r100 = load i32, ptr %r99
    %r101 = getelementptr [4 x [4 x i32]], ptr @map, i32 0, i32 %r97, i32 %r100
    %r102 = add i32 0,0
    store i32 %r102, ptr %r101
    %r103 = add i32 1,0
    %r106 = add i32 %r181,%r0
    br label %L18
L21:  ;
    %r111 = icmp ne i32 %r5,0
    br i1 %r111, label %L24, label %L25
L22:  ;
    %r159 = add i32 0,0
    %r160 = icmp eq i32 %r59,%r159
    br i1 %r160, label %L28, label %L29
L23:  ;
    %r190 = phi i32 [%r154,%L27],[%r188,%L30]
    %r185 = phi i32 [%r181,%L27],[%r184,%L30]
    %r176 = phi i32 [%r157,%L27],[%r179,%L30]
    br label %L15
L24:  ;
    %r112 = add i32 1,0
    ret i32 %r112
L25:  ;
    %r113 = add i32 0,0
    %r114 = getelementptr i32, ptr %r1, i32 %r113
    store i32 %r178, ptr %r114
    %r116 = add i32 0,0
    %r117 = getelementptr i32, ptr %r3, i32 %r116
    %r118 = load i32, ptr %r117
    %r119 = add i32 0,0
    %r120 = getelementptr i32, ptr %r4, i32 %r119
    %r121 = load i32, ptr %r120
    %r122 = getelementptr [4 x [4 x i32]], ptr @map, i32 0, i32 %r118, i32 %r121
    %r124 = add i32 1,0
    %r125 = add i32 %r71,%r124
    store i32 %r125, ptr %r122
    %r126 = add i32 0,0
    %r127 = getelementptr i32, ptr %r1, i32 %r126
    store i32 %r181, ptr %r127
    %r129 = add i32 0,0
    %r130 = getelementptr i32, ptr %r3, i32 %r129
    %r131 = load i32, ptr %r130
    %r132 = add i32 0,0
    %r133 = getelementptr i32, ptr %r4, i32 %r132
    %r134 = load i32, ptr %r133
    %r135 = getelementptr [4 x [4 x i32]], ptr @map, i32 0, i32 %r131, i32 %r134
    %r136 = add i32 0,0
    store i32 %r136, ptr %r135
    %r139 = add i32 1,0
    %r140 = add i32 %r71,%r139
    %r141 = getelementptr [20 x i32], ptr @LEN_OF_POW2, i32 0, i32 %r140
    %r142 = load i32, ptr %r141
    %r144 = load i32, ptr @max_num_len
    %r145 = icmp sgt i32 %r142,%r144
    br i1 %r145, label %L26, label %L27
L26:  ;
    store i32 %r142, ptr @max_num_len
    br label %L27
L27:  ;
    %r147 = load i32, ptr @score
    %r149 = add i32 1,0
    %r150 = add i32 %r71,%r149
    %r151 = getelementptr [20 x i32], ptr @POW2, i32 0, i32 %r150
    %r152 = load i32, ptr %r151
    %r153 = add i32 %r147,%r152
    store i32 %r153, ptr @score
    %r154 = add i32 1,0
    %r157 = add i32 %r178,%r0
    br label %L23
L28:  ;
    %r163 = add i32 %r181,%r0
    br label %L30
L29:  ;
    %r166 = add i32 %r178,%r0
    br label %L30
L30:  ;
    %r184 = phi i32 [%r163,%L28],[%r181,%L29]
    %r179 = phi i32 [%r178,%L28],[%r166,%L29]
    br label %L23
}
define void @generate()
{
L0:  ;
    br label %L1
L1:  ;
    %r1 = add i32 0,0
    %r3 = add i32 0,0
    %r5 = add i32 0,0
    %r7 = add i32 0,0
    %r9 = add i32 0,0
    br label %L2
L2:  ;
    %r66 = phi i32 [%r1,%L1],[%r38,%L7]
    %r64 = phi i32 [%r3,%L1],[%r65,%L7]
    %r63 = phi i32 [%r5,%L1],[%r62,%L7]
    %r59 = phi i32 [%r7,%L1],[%r58,%L7]
    %r55 = phi i32 [%r9,%L1],[%r54,%L7]
    %r11 = load i32, ptr @MAP_LEN
    %r12 = icmp slt i32 %r66,%r11
    br i1 %r12, label %L3, label %L4
L3:  ;
    %r13 = add i32 0,0
    br label %L5
L4:  ;
    %r40 = add i32 0,0
    %r41 = call i32 @rand()
    %r42 = add i32 2,0
    %r43 = srem i32 %r41,%r42
    %r44 = add i32 1,0
    %r45 = icmp slt i32 %r43,%r44
    br i1 %r45, label %L12, label %L13
L5:  ;
    %r65 = phi i32 [%r13,%L3],[%r35,%L9]
    %r62 = phi i32 [%r63,%L3],[%r61,%L9]
    %r58 = phi i32 [%r59,%L3],[%r57,%L9]
    %r54 = phi i32 [%r55,%L3],[%r53,%L9]
    %r15 = load i32, ptr @MAP_LEN
    %r16 = icmp slt i32 %r65,%r15
    br i1 %r16, label %L6, label %L7
L6:  ;
    %r19 = getelementptr [4 x [4 x i32]], ptr @map, i32 0, i32 %r66, i32 %r65
    %r20 = load i32, ptr %r19
    %r21 = add i32 0,0
    %r22 = icmp eq i32 %r20,%r21
    br i1 %r22, label %L8, label %L9
L7:  ;
    %r37 = add i32 1,0
    %r38 = add i32 %r66,%r37
    br label %L2
L8:  ;
    %r24 = add i32 1,0
    %r25 = add i32 %r54,%r24
    %r26 = call i32 @rand()
    %r28 = srem i32 %r26,%r25
    %r29 = add i32 0,0
    %r30 = icmp eq i32 %r28,%r29
    br i1 %r30, label %L10, label %L11
L9:  ;
    %r61 = phi i32 [%r62,%L6],[%r60,%L11]
    %r57 = phi i32 [%r58,%L6],[%r56,%L11]
    %r53 = phi i32 [%r54,%L6],[%r25,%L11]
    %r34 = add i32 1,0
    %r35 = add i32 %r65,%r34
    br label %L5
L10:  ;
    br label %L11
L11:  ;
    %r60 = phi i32 [%r62,%L8],[%r66,%L10]
    %r56 = phi i32 [%r58,%L8],[%r65,%L10]
    br label %L9
L12:  ;
    %r46 = add i32 1,0
    br label %L14
L13:  ;
    %r47 = add i32 2,0
    br label %L14
L14:  ;
    %r52 = phi i32 [%r46,%L12],[%r47,%L13]
    %r50 = getelementptr [4 x [4 x i32]], ptr @map, i32 0, i32 %r63, i32 %r59
    store i32 %r52, ptr %r50
    ret void
}
define void @move(i32 %r0)
{
L0:  ;
    %r3 = alloca [1 x i32]
    %r2 = alloca [1 x i32]
    br label %L1
L1:  ;
    %r5 = add i32 1,0
    %r7 = add i32 2,0
    %r8 = srem i32 %r0,%r7
    %r9 = add i32 2,0
    %r10 = mul i32 %r8,%r9
    %r11 = sub i32 %r5,%r10
    %r13 = add i32 0,0
    %r15 = add i32 2,0
    %r16 = sdiv i32 %r0,%r15
    %r17 = add i32 0,0
    %r18 = icmp eq i32 %r16,%r17
    br i1 %r18, label %L2, label %L3
L2:  ;
    %r20 = getelementptr [1 x i32], ptr %r2, i32 0
    %r21 = getelementptr [1 x i32], ptr %r3, i32 0
    %r22 = getelementptr [1 x i32], ptr %r2, i32 0
    %r23 = getelementptr [1 x i32], ptr %r3, i32 0
    %r24 = add i32 0,0
    %r25 = call i32 @move_base(i32 %r11,ptr %r20,ptr %r21,ptr %r22,ptr %r23,i32 %r24)
    br label %L4
L3:  ;
    %r27 = getelementptr [1 x i32], ptr %r3, i32 0
    %r28 = getelementptr [1 x i32], ptr %r2, i32 0
    %r29 = getelementptr [1 x i32], ptr %r2, i32 0
    %r30 = getelementptr [1 x i32], ptr %r3, i32 0
    %r31 = add i32 0,0
    %r32 = call i32 @move_base(i32 %r11,ptr %r27,ptr %r28,ptr %r29,ptr %r30,i32 %r31)
    br label %L4
L4:  ;
    %r40 = phi i32 [%r25,%L2],[%r32,%L3]
    %r34 = icmp eq i32 %r40,0
    br i1 %r34, label %L5, label %L6
L5:  ;
    %r35 = getelementptr [26 x i32], ptr @STR_INVALID, i32 0
    call void @put_string(ptr %r35)
    %r36 = load i32, ptr @CHR_ENTER
    call void @putch(i32 %r36)
    ret void
L6:  ;
    %r37 = load i32, ptr @step
    %r38 = add i32 1,0
    %r39 = add i32 %r37,%r38
    store i32 %r39, ptr @step
    call void @generate()
    call void @print_map()
    ret void
}
define i32 @try_move()
{
L0:  ;
    %r1 = alloca [1 x i32]
    %r0 = alloca [1 x i32]
    br label %L1
L1:  ;
    %r2 = add i32 1,0
    %r3 = getelementptr [1 x i32], ptr %r0, i32 0
    %r4 = getelementptr [1 x i32], ptr %r1, i32 0
    %r5 = getelementptr [1 x i32], ptr %r0, i32 0
    %r6 = getelementptr [1 x i32], ptr %r1, i32 0
    %r7 = add i32 1,0
    %r8 = call i32 @move_base(i32 %r2,ptr %r3,ptr %r4,ptr %r5,ptr %r6,i32 %r7)
    %r9 = add i32 1,0
    %r10 = getelementptr [1 x i32], ptr %r1, i32 0
    %r11 = getelementptr [1 x i32], ptr %r0, i32 0
    %r12 = getelementptr [1 x i32], ptr %r0, i32 0
    %r13 = getelementptr [1 x i32], ptr %r1, i32 0
    %r14 = add i32 1,0
    %r15 = call i32 @move_base(i32 %r9,ptr %r10,ptr %r11,ptr %r12,ptr %r13,i32 %r14)
    %r16 = call i32 @or(i32 %r8,i32 %r15)
    %r17 = add i32 1,0
    %r18 = sub i32 0,%r17
    %r19 = getelementptr [1 x i32], ptr %r0, i32 0
    %r20 = getelementptr [1 x i32], ptr %r1, i32 0
    %r21 = getelementptr [1 x i32], ptr %r0, i32 0
    %r22 = getelementptr [1 x i32], ptr %r1, i32 0
    %r23 = add i32 1,0
    %r24 = call i32 @move_base(i32 %r18,ptr %r19,ptr %r20,ptr %r21,ptr %r22,i32 %r23)
    %r25 = add i32 1,0
    %r26 = sub i32 0,%r25
    %r27 = getelementptr [1 x i32], ptr %r1, i32 0
    %r28 = getelementptr [1 x i32], ptr %r0, i32 0
    %r29 = getelementptr [1 x i32], ptr %r0, i32 0
    %r30 = getelementptr [1 x i32], ptr %r1, i32 0
    %r31 = add i32 1,0
    %r32 = call i32 @move_base(i32 %r26,ptr %r27,ptr %r28,ptr %r29,ptr %r30,i32 %r31)
    %r33 = call i32 @or(i32 %r24,i32 %r32)
    %r34 = call i32 @or(i32 %r16,i32 %r33)
    ret i32 %r34
}
define i32 @main()
{
L0:  ;
    br label %L1
L1:  ;
    %r0 = getelementptr [25 x i32], ptr @STR_INIT, i32 0
    call void @put_string(ptr %r0)
    %r1 = call i32 @getint()
    store i32 %r1, ptr @seed
    call void @init()
    call void @generate()
    call void @print_map()
    br label %L2
L2:  ;
    %r2 = load i32, ptr @alive
    %r3 = icmp ne i32 %r2,0
    br i1 %r3, label %L3, label %L4
L3:  ;
    %r5 = call i32 @getch()
    %r7 = add i32 119,0
    %r8 = icmp eq i32 %r5,%r7
    br i1 %r8, label %L5, label %L6
L4:  ;
    %r58 = add i32 0,0
    ret i32 %r58
L5:  ;
    %r9 = load i32, ptr @UP
    call void @move(i32 %r9)
    br label %L7
L6:  ;
    %r11 = add i32 97,0
    %r12 = icmp eq i32 %r5,%r11
    br i1 %r12, label %L8, label %L9
L7:  ;
    %r55 = call i32 @try_move()
    %r56 = icmp eq i32 %r55,0
    br i1 %r56, label %L32, label %L33
L8:  ;
    %r13 = load i32, ptr @LEFT
    call void @move(i32 %r13)
    br label %L10
L9:  ;
    %r15 = add i32 115,0
    %r16 = icmp eq i32 %r5,%r15
    br i1 %r16, label %L11, label %L12
L10:  ;
    br label %L7
L11:  ;
    %r17 = load i32, ptr @DOWN
    call void @move(i32 %r17)
    br label %L13
L12:  ;
    %r19 = add i32 100,0
    %r20 = icmp eq i32 %r5,%r19
    br i1 %r20, label %L14, label %L15
L13:  ;
    br label %L10
L14:  ;
    %r21 = load i32, ptr @RIGHT
    call void @move(i32 %r21)
    br label %L16
L15:  ;
    %r23 = add i32 104,0
    %r24 = icmp eq i32 %r5,%r23
    br i1 %r24, label %L17, label %L18
L16:  ;
    br label %L13
L17:  ;
    %r25 = getelementptr [62 x i32], ptr @STR_HELP, i32 0
    call void @put_string(ptr %r25)
    br label %L19
L18:  ;
    %r27 = add i32 113,0
    %r28 = icmp eq i32 %r5,%r27
    br i1 %r28, label %L20, label %L23
L19:  ;
    br label %L16
L20:  ;
    %r33 = getelementptr [12 x i32], ptr @STR_GG, i32 0
    call void @put_string(ptr %r33)
    %r34 = add i32 0,0
    ret i32 %r34
L21:  ;
    %r36 = add i32 112,0
    %r37 = icmp eq i32 %r5,%r36
    br i1 %r37, label %L24, label %L25
L22:  ;
    br label %L19
L23:  ;
    %r30 = add i32 1,0
    %r31 = sub i32 0,%r30
    %r32 = icmp eq i32 %r5,%r31
    br i1 %r32, label %L20, label %L21
L24:  ;
    %r38 = load i32, ptr @CHR_ENTER
    call void @putch(i32 %r38)
    call void @print_map()
    br label %L26
L25:  ;
    %r40 = add i32 32,0
    %r41 = icmp eq i32 %r5,%r40
    br i1 %r41, label %L27, label %L31
L26:  ;
    br label %L22
L27:  ;
    br label %L2
L28:  ;
    %r48 = getelementptr [26 x i32], ptr @STR_INVALID, i32 0
    call void @put_string(ptr %r48)
    %r49 = load i32, ptr @CHR_ENTER
    call void @putch(i32 %r49)
    %r50 = load i32, ptr @seed
    %r52 = add i32 %r50,%r5
    %r53 = add i32 1073741824,0
    %r54 = srem i32 %r52,%r53
    store i32 %r54, ptr @seed
    br label %L29
L29:  ;
    br label %L26
L30:  ;
    %r46 = add i32 13,0
    %r47 = icmp eq i32 %r5,%r46
    br i1 %r47, label %L27, label %L28
L31:  ;
    %r43 = add i32 10,0
    %r44 = icmp eq i32 %r5,%r43
    br i1 %r44, label %L27, label %L30
L32:  ;
    %r57 = getelementptr [12 x i32], ptr @STR_GG, i32 0
    call void @put_string(ptr %r57)
    br label %L4
L33:  ;
    br label %L2
}
