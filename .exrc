if &cp | set nocp | endif
let s:cpo_save=&cpo
set cpo&vim
inoremap <C-F11> :Maximize
inoremap <F11> :Fullscreen
nmap <silent> # <Plug>MarkSearchPrev
nnoremap ' "
nmap <silent> * <Plug>MarkSearchNext
nnoremap ; :
nmap <silent> \? <Plug>MarkSearchAnyPrev
nmap <silent> \/ <Plug>MarkSearchAnyNext
nmap <silent> \# <Plug>MarkSearchCurrentPrev
nmap <silent> \* <Plug>MarkSearchCurrentNext
nmap <silent> \n <Plug>MarkClear
vmap <silent> \r <Plug>MarkRegex
nmap <silent> \r <Plug>MarkRegex
vmap <silent> \m <Plug>MarkSet
nmap <silent> \m <Plug>MarkSet
nmap <silent> \ucs :call C_RemoveGuiMenus()
nmap <silent> \lcs :call C_CreateGuiMenus()
nmap <silent> \ubs :call BASH_RemoveGuiMenus()
nmap <silent> \lbs :call BASH_CreateGuiMenus()
nmap \ihn :IHN
nmap \is :IHS:A
nmap \ih :IHS
map \a :A
nnoremap \' '
noremap \dm mmHmn:%s///ge'nzt'm
nmap <silent> \tl :Tlist
map \yr :YRShow
map <silent> \ee :call SwitchToBuf("~/.vimrc")
map <silent> \ss :source ~/.vimrc
nmap gx <Plug>NetrwBrowseX
map tg :!ctags -R  --languages=c++ --c++-kinds=+p --fields=+iaS --extra=+q .
nnoremap <silent> <Plug>NetrwBrowseX :call netrw#NetrwBrowseX(expand("<cWORD>"),0)
nnoremap <C-F11> :Maximize
nnoremap <silent> <Plug>MarkSearchPrev :if !mark#SearchNext(1)|execute 'normal! #zv'|endif
nnoremap <silent> <Plug>MarkSearchNext :if !mark#SearchNext(0)|execute 'normal! *zv'|endif
nnoremap <silent> <Plug>MarkSearchAnyPrev :call mark#SearchAnyMark(1)
nnoremap <silent> <Plug>MarkSearchAnyNext :call mark#SearchAnyMark(0)
nnoremap <silent> <Plug>MarkSearchCurrentPrev :call mark#SearchCurrentMark(1)
nnoremap <silent> <Plug>MarkSearchCurrentNext :call mark#SearchCurrentMark(0)
nnoremap <silent> <Plug>MarkToggle :call mark#Toggle()
nnoremap <silent> <Plug>MarkAllClear :call mark#ClearAll()
nnoremap <silent> <Plug>MarkClear :call mark#DoMark(mark#CurrentMark()[0])
vnoremap <silent> <Plug>MarkRegex :call mark#MarkRegex(mark#GetVisualSelectionAsRegexp())
nnoremap <silent> <Plug>MarkRegex :call mark#MarkRegex('')
vnoremap <silent> <Plug>MarkSet :call mark#DoMark(mark#GetVisualSelectionAsLiteralPattern())
nnoremap <silent> <Plug>MarkSet :call mark#MarkCurrentWord()
nnoremap <F11> :Fullscreen
map <F3> :A
map <F2> k :q
map <F6> :NERDTreeClose
map <F5> :NERDTree
map <F4> :nohlsearch
inoremap <expr>  omni#cpp#maycomplete#Complete()
inoremap <expr> . omni#cpp#maycomplete#Dot()
inoremap <expr> : omni#cpp#maycomplete#Scope()
inoremap <expr> > omni#cpp#maycomplete#Arrow()
imap \ihn :IHN
imap \is :IHS:A
imap \ih :IHS
iabbr brif @brief:
iabbr todo //TODO:
iabbr dl << std::endl ;
iabbr cn std::cin >>
iabbr ct std::cout <<
iabbr ty typename 
iabbr tm template 
iabbr po protected:
iabbr pi private:
iabbr pu public:
iabbr re return ;
let &cpo=s:cpo_save
unlet s:cpo_save
set autoindent
set autoread
set autowrite
set background=dark
set backspace=eol,start,indent
set cindent
set cmdheight=2
set cscopeprg=/usr/bin/cscope
set cscopetag
set cscopetagorder=1
set cscopeverbose
set fileencodings=utf-8,gb2312,gbk,gb18030
set grepprg=grep\ -nH\ $*
set helplang=en
set hlsearch
set ignorecase
set incsearch
set laststatus=2
set lazyredraw
set mouse=a
set omnifunc=omni#cpp#complete#Main
set pastetoggle=<F3>
set printoptions=paper:letter
set ruler
set runtimepath=~/.vim,/var/lib/vim/addons,/usr/share/vim/vimfiles,/usr/share/vim/vim73,/usr/share/vim/vimfiles/after,/var/lib/vim/addons/after,~/.vim/after
set sessionoptions=blank,buffers,folds,help,options,tabpages,winsize,sesdir
set shiftwidth=4
set smartcase
set smartindent
set smarttab
set suffixes=.bak,~,.swp,.o,.info,.aux,.log,.dvi,.bbl,.blg,.brf,.cb,.ind,.idx,.ilg,.inx,.out,.toc
set termencoding=utf-8
set viminfo='10,\"100,:20,n~/.viminfo
set whichwrap=b,s,<,>
set wildmenu
set window=47
set nowrapscan
set nowritebackup
" vim: set ft=vim :
