    .text
    .global f

f:
      // A * x^2
      mul   r0, r0, r3
      mul   r0, r0, r3
      // B * x
      mul   r1, r1, r3
      // A * x^2
      add   r0, r0, r1
      add   r0, r0, r2
      
      bx    lr
