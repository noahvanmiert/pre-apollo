" Vim syntax file
" Language: Apollo

" Usage Instructions
" Put this file in .vim/syntax/apollo.vim
" and add in your .vimrc file the next line:
" autocmd BufRead,BufNewFile *.apo set filetype=apollo

if exists("b:current_syntax")
  finish
endif

set iskeyword=a-z,A-Z
syntax keyword apolloTodos TODO XXX FIXME NOTE

" Language keywords
syntax keyword apolloKeywords fun let true false

" Comments
syntax region apolloCommentLine start="//" end="$"   contains=apolloTodos

" String literals
syntax region apolloString start=/\v"/ skip=/\v\\./ end=/\v"/ contains=apolloEscapes

" Char literals
syntax region apolloChar start=/\v'/ skip=/\v\\./ end=/\v'/ contains=apolloEscapes

" Escape literals \n, \r, ....
syntax match apolloEscapes display contained "\\[nr\"']"

" Type names the compiler recognizes
syntax keyword apolloTypeNames string int bool

" Set highlights
highlight default link apolloTodos Todo
highlight default link apolloKeywords Keyword
highlight default link apolloCommentLine Comment
highlight default link apolloString String
highlight default link apolloTypeNames Type
highlight default link apolloChar Character
highlight default link apolloEscapes SpecialChar

let b:current_syntax = "apollo"
