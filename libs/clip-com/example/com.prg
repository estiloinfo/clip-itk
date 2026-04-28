port:="/dev/ttyUSB0" 

fd:=COM_OPEN(port)

? fd

IF fd = -1
     ? "cannot opened port !", FERRORSTR()
     break
ENDIF

? "**********************" 

? 'init', COM_INIT(fd, 9600, 'N', 8, 1)

? "**********************" 

sleep( 3 )

msg:='b'
? 'send', COM_SEND(fd, msg)
? 'read', COM_READ(fd)

sleep(3)

? 'send', COM_SEND(fd, "a")
? 'read', COM_READ(fd)
? 'send', COM_SEND(fd, "v")
? 'read', COM_READ(fd)

? COM_CLOSE(fd)

?
