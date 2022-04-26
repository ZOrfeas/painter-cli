	.section	__TEXT,__text,regular,pure_instructions
	.build_version macos, 12, 0
	.globl	_main                           ; -- Begin function main
	.p2align	2
_main:                                  ; @main
	.cfi_startproc
; %bb.0:
	sub	sp, sp, #48                     ; =48
	stp	x29, x30, [sp, #32]             ; 16-byte Folded Spill
	add	x29, sp, #32                    ; =32
	.cfi_def_cfa w29, 16
	.cfi_offset w30, -8
	.cfi_offset w29, -16
	mov	w8, #0
	str	w8, [sp, #12]                   ; 4-byte Folded Spill
	stur	wzr, [x29, #-4]
	adrp	x8, l___const.main.tmp@PAGE
	ldrb	w8, [x8, l___const.main.tmp@PAGEOFF]
	sturb	w8, [x29, #-5]
	adrp	x8, l_.str@PAGE
	add	x8, x8, l_.str@PAGEOFF
	mov	x9, x8
	str	x9, [sp, #16]
	adrp	x0, l_.str.1@PAGE
	add	x0, x0, l_.str.1@PAGEOFF
	mov	x9, sp
	str	x8, [x9]
	bl	_printf
	ldr	w0, [sp, #12]                   ; 4-byte Folded Reload
	ldp	x29, x30, [sp, #32]             ; 16-byte Folded Reload
	add	sp, sp, #48                     ; =48
	ret
	.cfi_endproc
                                        ; -- End function
	.section	__TEXT,__const
l___const.main.tmp:                     ; @__const.main.tmp
	.space	1

	.section	__TEXT,__cstring,cstring_literals
l_.str:                                 ; @.str
	.asciz	"tmp"

l_.str.1:                               ; @.str.1
	.asciz	"%s\n"

.subsections_via_symbols
