	.text
	.globl main
	.attribute arch, "rv64i2p1_m2p0_a2p1_f2p2_d2p2_c2p0_zicsr2p0_zifencei2p0_zba1p0_zbb1p0"
main:
.main_0:
	addi		sp,sp,-16
	jal			x0,.main_1
.main_1:
	addiw		t0,x0,0
	addiw		t0,x0,0
	addiw		t0,x0,0
	addiw		t0,x0,0
	addiw		t0,x0,0
	addiw		t0,x0,5
	addiw		t1,x0,5
	addiw		t2,x0,1
	addiw		t3,x0,2
	addiw		t4,x0,0
	subw		t3,t4,t3
	addiw		t4,x0,2
	addiw		t4,x0,1
	mulw		t3,t3,t4
	addiw		t4,x0,2
	divw		t3,t3,t4
	addiw		t4,x0,0
	blt			t3,t4,.main_2
	jal			x0,.main_4
.main_2:
	jal			x0,.main_3
.main_3:
	addiw		a0,x0,0
	addi		sp,sp,16
	jalr		x0,ra,0
.main_4:
	subw		t0,t0,t1
	addiw		t1,x0,0
	bne			t0,t1,.main_5
	jal			x0,.main_3
.main_5:
	addiw		t0,x0,3
	addw		t0,t2,t0
	addiw		t1,x0,2
	remw		t0,t0,t1
	addiw		t1,x0,0
	bne			t0,t1,.main_2
	jal			x0,.main_3
	.data
