\NeedsTeXFormat{LaTeX2e}
\ProvidesClass{styles/diploma}[2026/05/10 ITMO LaTeX diploma class]
\def\my@fontsize{12pt}\def\my@baseclass{article}\def\my@title12pt{}\def\my@body14pt{}

% Если используется опция 14pt то нужно несколько хаков
\DeclareOption{14pt}{
  \def\ignore#1{}
  % во-первых, класс наследуется от extarticle
  \def\my@fontsize{14pt}
  \def\my@baseclass{extarticle}
  
  % во-вторых, титульный лист должен набираться так, как будто
  % используется шрифт 12pt - иначе он выглядит совсем конским
  % Для этого мы явно загружаем size12.clo, но так как он на такое не рассчитан,
  % и использует \newcommand для определения всяких \small и \Large
  % то у нас \newcommand временно превращается в \renewcommand
  \let\orignewcommand\newcommand
  \let\origNeedsTeXFormat\NeedsTeXFormat
  \def\my@title12pt{
    \let\newcommand\renewcommand
    \makeatletter
    \input{size12.clo}%
    \makeatother  
    \let\newcommand\orignewcommand
  }
  % в-третьих, после того как титульный лист сгенерирован, нужно снова вернуться
  % к базовому шрифту 14pt. Применяется такой же хак: загрузи size14.clo 
  % временно переопределив \newcommand => \renewcommand
  \def\my@body14pt{
    % отдельное внимание подавлению команды \NeedsTeXFormat, которая требует
    % чтобы код size14.clo выполнялся в преамбуле (а тут он уже внутри документа)
    \let\NeedsTeXFormat\ignore
    \let\newcommand\renewcommand
    \makeatletter
    \input{size14.clo}%
    \makeatother  
    \let\newcommand\orignewcommand
    \let\NeedsTeXFormat\origNeedsTeXFormat
  }
}
\ProcessOptions\relax
\LoadClass[a4paper,\my@fontsize]{\my@baseclass}

% Поля настраиваются командами \newgeometry во время и после генерации титульных 
% страниц и 
\usepackage[a4paper]{geometry}

\usepackage{fancyhdr}
\usepackage{ifthen}
%% Мы используем компилятор XeLaTex, который дружит с Unicode и True Type/Open Type
%% шрифтами. Для русификации достаточно подключить пакет fontspec и
%% выбрать Unicode шрифт в котором есть кириллические глифы. Ниже
%% основным шрифтом выбирается Unicode версия шрифта Computer Modern с заcечками
\usepackage{fontspec}
\setmainfont[Mapping=tex-text]{CMU Serif}
\newfontfamily{\cyrillicfonttt}{CMU Typewriter Text}

%% В XeLaTex заменой известного пакета babel является пакет polyglossia.
%% Теперь у нас будут переносы слов
\usepackage{polyglossia}
\setdefaultlanguage[spelling=modern]{russian}

%% Резервный люфт для строк, чтобы небольшие overfull не вылезали за поле
\emergencystretch=2em
%% Отступ в первом абзаце
\usepackage{indentfirst}
%% Гиперссылки
\usepackage[colorlinks=true,urlcolor=black,linkcolor=black,filecolor=black,citecolor=black]{hyperref}

%% Колдунство с оформлением приложений
\usepackage[titletoc, title]{appendix}
\patchcmd{\appendices}{\quad}{. }{}{}

%% Каждая глава начинается с новой страницы, введение и заключение не
%% нумеруются, но в оглавление входят. А само оглавление в оглавление не входит
\let\old@section\section
\def\section{\@ifstar\@section\@@section}
\def\@section#1{\newpage\old@section*{#1}%
        \ifthenelse{\equal{#1}{\contentsname}}%
           {}%
           {\addcontentsline{toc}{section}{#1}}%
}
\def\@@section#1{\newpage\old@section{#1}}

%% Полуторный интервал
\usepackage[nodisplayskipstretch]{setspace}
\onehalfspacing

%% Переименование "содержания" в "оглавление"
\gappto\captionsrussian{\renewcommand{\contentsname}{Оглавление}}

\let\savenumberline\numberline
\def\numberline#1{\savenumberline{#1.}}

%% Для более гибких подписей к рисункам и прочей фигне
\usepackage[justification=centering]{caption}

%% Настраиваем minted для подсветки кода
\usepackage[cache=false]{minted}
\usepackage{listings}
\setminted{xleftmargin=\parindent, linenos, autogobble, frame=lines, escapeinside=\#\#}
\renewcommand{\theFancyVerbLine}{\arabic{FancyVerbLine}}

%% Теоремы, леммы, определения, др. окружения
\usepackage{amsmath}
\usepackage{amsthm}

\theoremstyle{definition}
\newtheorem{definition}{Определение}[section]

%% Пользовательские команды
\newcommand{\code}[1]{\texttt{#1}}
\newcommand{\term}[1]{\textbf{#1}}

%% Отключаем назойливые warning'и про overfull hbox
% \hfuzz=100pt 

%% Добавляем нумерованные параграфы

\renewcommand\paragraph{\@startsection{paragraph}{4}{\z@}%
            {-2.5ex\@plus -1ex \@minus -.25ex}%
            {1.25ex \@plus .25ex}%
            {\normalfont\normalsize\bfseries}}

% \setcounter{secnumdepth}{4} % how many sectioning levels to assign numbers to
% \setcounter{tocdepth}{4}    % how many sectioning levels to show in ToC