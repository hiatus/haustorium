.section .rodata, "a"
.balign 8

.global _mod_start
.global _mod_end

_mod_start:
	.incbin MODBIN
_mod_end:
