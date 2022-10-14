        # This is the inner loop of mandel_ref
        # Parameters are passed to the function as follows:
        #   %xmm0: c_re
        #   %xmm1: c_im
        #   %edi:  count
        # Before entering the loop, the function sets registers
	# to initialize local variables:
        #   %xmm2: z_re = c_re
        #   %xmm3: z_im = c_im
        #   %eax:  i = 0
.L123:
        vmulss  %xmm2, %xmm2, %xmm4
        vmulss  %xmm3, %xmm3, %xmm5
        vaddss  %xmm5, %xmm4, %xmm6
        vucomiss        .LC0(%rip), %xmm6
        ja      .L126
        vaddss  %xmm2, %xmm2, %xmm2
        addl    $1, %eax
        cmpl    %edi, %eax   # Set condition codes for jne below
        vmulss  %xmm3, %xmm2, %xmm3
        vsubss  %xmm5, %xmm4, %xmm2
        vaddss  %xmm3, %xmm1, %xmm3
        vaddss  %xmm2, %xmm0, %xmm2
        jne     .L123
