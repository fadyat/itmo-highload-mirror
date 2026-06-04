$pdf_mode = 5;             # XeLaTeX
$out_dir = 'build';
$bibtex_use = 2;
$pdf_previewer = 'open';
ensure_path('TEXINPUTS', './styles');
ensure_path('BIBINPUTS', './bib');
ensure_path('BSTINPUTS', './bib');
$ENV{PATH} = '/opt/homebrew/bin:/usr/local/bin:' . $ENV{PATH};
set_tex_cmds('--shell-escape -synctex=1 -interaction=nonstopmode %O %S');

add_cus_dep('glo', 'gls', 0, 'makeglossaries');
add_cus_dep('acn', 'acr', 0, 'makeglossaries');
sub makeglossaries {
    my ($base_name, $path) = fileparse($_[0]);
    pushd $path;
    my $return = system "makeglossaries", $base_name;
    popd;
    return $return;
}
push @generated_exts, 'glo', 'gls', 'glg', 'acn', 'acr', 'alg';
$clean_ext .= ' %R.ist %R.xdy';

add_cus_dep('mmd', 'pdf', 0, 'mermaid');
sub mermaid {
    my $base = $_[0];
    return system 'npx', '-p', '@mermaid-js/mermaid-cli', 'mmdc',
        '-i', "$base.mmd", '-o', "$base.pdf",
        '-t', 'neutral', '-b', 'transparent',
        '--pdfFit';
}
