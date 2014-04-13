        ;; root (/) node

        ;; /dev/ node
        db 'dev', 0, 0, 0, 0, 0, 0, 0, 0, 0 ; name
        db 3                              ; exists, not a directory
        db 2                              ; sector 2
        db 0                              ; head 0
        db 1                              ; cylinder 1

        ;; /csug/ dir
        db 'csug', 0, 0, 0, 0, 0, 0, 0, 0 ; name
        db 3                              ; exists, is a directory
        db 4                              ; sector 3
        db 0                              ; head 0
        db 1                              ; cylinder 1

        ;; No other entries
        times 29 dd 0, 0, 0, 0

        db 6                    ; last sector
        db 0                    ; last head
        db 1                    ; last cylinder
        db 0                    ; unused

        dd 0, 0, 0             ; padding

        ;; /dev/ node
        db 'random', 0, 0, 0, 0, 0, 0 ; name
        db 1                            ; exists, not a directory
        db 6                            ; sector 4
        db 0                            ; head 0
        db 1                            ; cylinder 1

        times 30 dd 0, 0, 0, 0

        dd 0, 0, 0, 0           ; next sector, etc only on root

        times 512 db 0

        ;; /csug/ node
        db 'hello', 0, 0, 0, 0, 0, 0, 0 ; name
        db 1                            ; exists, not a directory
        db 5                            ; sector 4
        db 0                            ; head 0
        db 1                            ; cylinder 1

        times 30 dd 0, 0, 0, 0

        dd 0, 0, 0, 0           ; next sector, etc only on root

        ;; /csug/hello file
        db 'this is not a potato', 0
        times 491 db 0

        ;; /dev/random file
        db '4 // guaranteed by fair dice roll', 0
        times 478 db 0

        times 512 db 0
        times 512 db 0
        times 512 db 0
        times 512 db 0
