let SessionLoad = 1
let s:so_save = &g:so | let s:siso_save = &g:siso | setg so=0 siso=0 | setl so=-1 siso=-1
let v:this_session=expand("<sfile>:p")
silent only
silent tabonly
cd ~/dev/c/terminal_emulator
if expand('%') == '' && !&modified && line('$') <= 1 && getline(1) == ''
  let s:wipebuf = bufnr('%')
endif
set shortmess=aoO
badd +1 ~/dev/c/terminal_emulator
badd +212 pty.c
badd +11 pty2.c
badd +92 man://openpty(3)
badd +15 man://posix_openpt(3)
badd +3 man://Fatal(3perl)
badd +80 man://perror(3)
badd +11 term://~/dev/c/terminal_emulator//25254:/bin/bash
badd +0 man://termios(3)
argglobal
%argdel
$argadd ~/dev/c/terminal_emulator
edit pty.c
let s:save_splitbelow = &splitbelow
let s:save_splitright = &splitright
set splitbelow splitright
wincmd _ | wincmd |
split
1wincmd k
wincmd w
let &splitbelow = s:save_splitbelow
let &splitright = s:save_splitright
wincmd t
let s:save_winminheight = &winminheight
let s:save_winminwidth = &winminwidth
set winminheight=0
set winheight=1
set winminwidth=0
set winwidth=1
exe '1resize ' . ((&lines * 21 + 22) / 44)
exe '2resize ' . ((&lines * 20 + 22) / 44)
argglobal
setlocal fdm=manual
setlocal fde=nvim_treesitter#foldexpr()
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=0
setlocal fml=1
setlocal fdn=20
setlocal fen
silent! normal! zE
let &fdl = &fdl
let s:l = 214 - ((10 * winheight(0) + 10) / 21)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 214
normal! 0
lcd ~/dev/c/terminal_emulator
wincmd w
argglobal
if bufexists("term://~/dev/c/terminal_emulator//25254:/bin/bash") | buffer term://~/dev/c/terminal_emulator//25254:/bin/bash | else | edit term://~/dev/c/terminal_emulator//25254:/bin/bash | endif
if &buftype ==# 'terminal'
  silent file term://~/dev/c/terminal_emulator//25254:/bin/bash
endif
balt ~/dev/c/terminal_emulator/pty.c
setlocal fdm=manual
setlocal fde=nvim_treesitter#foldexpr()
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=0
setlocal fml=1
setlocal fdn=20
setlocal fen
let s:l = 4 - ((3 * winheight(0) + 10) / 20)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 4
normal! 0
lcd ~/dev/c/terminal_emulator
wincmd w
exe '1resize ' . ((&lines * 21 + 22) / 44)
exe '2resize ' . ((&lines * 20 + 22) / 44)
tabnext 1
if exists('s:wipebuf') && len(win_findbuf(s:wipebuf)) == 0&& getbufvar(s:wipebuf, '&buftype') isnot# 'terminal'
  silent exe 'bwipe ' . s:wipebuf
endif
unlet! s:wipebuf
set winheight=1 winwidth=20 shortmess=c
let &winminheight = s:save_winminheight
let &winminwidth = s:save_winminwidth
let s:sx = expand("<sfile>:p:r")."x.vim"
if filereadable(s:sx)
  exe "source " . fnameescape(s:sx)
endif
let &g:so = s:so_save | let &g:siso = s:siso_save
set hlsearch
nohlsearch
doautoall SessionLoadPost
unlet SessionLoad
" vim: set ft=vim :
