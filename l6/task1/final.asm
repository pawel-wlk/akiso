                opt f-g-h+l+o+
                org $1000

start           equ *

                lda <text
                sta $80
                lda >text
                sta $81
                ldy #1
                lda #$e5
                jsr phex

                lda <text
                ldx >text
                jsr $ff80
                brk

PHEX            PHA
                JSR PXDIG
                PLA
                LSR @
                LSR @
                LSR @
                LSR @
PXDIG           AND #%00001111
                ORA #'0'
                CMP #'9'+1
                BCC PR
                ADC #'A'-'9'-2
PR              STA ($80),Y
                DEY
                RTS
byte            dta b(0)

                org $2000
text            equ *
                dta a(0),a(0),a(0),a(0)
                dta b(10) ; '\n'
                dta a(0)

                org $2E0
                dta a(start)

                end of file



