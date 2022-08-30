#!powershell -Sta -NonInteractive -NoProfile -NoLogo -ExecutionPolicy RemoteSigned %~dpn0.ps1 %*

$default_define=@{"MAX_PATH"=260;"MAXPATHLEN"=256;"LF_FACESIZE"=32;"MAX_UIMSG"=1024;}
$type_lst=@("tttset","TComVar")   #,"TGetHNRec","TKeyMap"
$out_file="../ttxversion_db.h"
$subst_lst=@("eterm_lookfeel_t","eterm_lookfeel2_t","cygterm_t","cygterm2_t")
$struct1_lst=$subst_lst+@("tttset")
$struct2_lst=@("TComVar")
$struct_lst=$subst_lst+$type_lst
#$struct_lst=$subst_lst+@("tttset","TGetHNRec","TComVar")

#### source set
filter Get-SourceVersion{
    if($_ -match "v(\d+)_(\d+)([A-Za-z])?_\w+\.h"){(0+$Matches[1])*1000+(0+$Matches[2])}
}
filter Get-SourceSet ($path){begin{$va=0;$vb=0}process{
    $v1=[int]($_/1000);$v2=$_%1000;
    if(Test-Path ("${path}/v${v1}_${v2}*_teraterm.h")){$va=$_}
    if(Test-Path ("${path}/v${v1}_${v2}*_tttypes.h")){$vb=$_}
    $va1=[int]($va/1000);$va2=$va%1000;
    $vb1=[int]($vb/1000);$vb2=$vb%1000;
    $pa="${path}/v${va1}_${va2}_teraterm.h"
    $pb="${path}/v${vb1}_${vb2}_tttypes.h"
    @{v=$_;pa=$pa;pb=$pb}
}}
####
filter Get-CStructName{begin{$s=0;$n=""}process{
    if($_ -match "struct[^{]*{"){$s=1;$n=$_
    }elseif($s -and $_ -match "}"){$s=0;
        if(($n+$_) -match "(\w+)([\{\};]|\s)*$"){$Matches[1]}}
}}
filter Remove-CComment{begin{$s=0}process{$v=$_
    if($s -eq 1){if($v -match "\*/"){$v=$v -replace "^.*\*/","";$s=0}}
    if($s -eq 0){
        $v=$v -replace "//.*$","" -replace "/\*.*\*/",""
        if ($v -match "/\*"){$s=1;$v=$v -replace "/\*.*$",""}
        $v=$v.trim()
        if($v.length -ne 0){$v}
    }
}}
filter Remove-CElse{begin{$s=0}process{
    if($_ -match "#else"){$s=1}elseif($_ -match "#endif"){$s=0}elseif($s -eq 0){$_}
}}
filter Format-CStruct($col=@()){begin{$s=0;$i=0}process{
    if($_ -match "struct[^{]*{"){$s=$s+1
    }elseif($s -and $_ -match "}"){$s=$s-1;if($s -eq 0){$i=$i+1}
    }elseif($s -and $_ -match "^\s*([^/;#]+)"){$v=$Matches[1].trim();$vv=$v
        $v=$v -replace "FAR\s*\*","*"
        $v=$v -replace "(const|unsigned|struct|enum)\s+(\w+)","`$1_`$2"
        $v=$v -replace "\s+(\*+)","`$1 "
        $t,$v=$v -split " ",2
        if($v -match "\(\*(\w+)"){
             $t=$t+($v -replace $Matches[1],"" -replace "\s+\w+","" -replace "\s+","")
             $v=$Matches[1]
        }
        $v=$v -replace "\s+",""
        $v.Split(",")|%{$col[$i]+" "+$t+" "+$_}
    }
}}
filter Format-CArray{$_ -replace "(\w+)\s(\w+)\[((?:\d+|\w+|[-()*+])+)\]","`$1 `$2[] `$3" -replace "\[(\w+)\]","*`$1"}
function CStructMember($v){
    $v=$v -replace "\]\s+\[","][" -replace "FAR\s*\*","*"
    $v=$v -replace "\s+[*]\s*|[*]\s+","* " -replace "\s*,\s*",","
    $v=$v -replace "\s+"," "
    $v=$v -replace "(const|unsigned|struct|enum)\s+","`$1."
    if($v -match "\(\*(\w+)"){
        $v=$v -replace $Matches[1],"" -replace "\s+\w+","" -replace "\s+",""
        $v=$v+" "+$Matches[1]
        if($v){$v}
    }else{
        $vs=$v.Split(" ,")
        $vs[1..$vs.count]|%{"{0} {1}" -f $vs[0],$_}
    }
}
filter Get-CDefine($m_base){
    begin{$m=$m_base.clone()}
    process{if($_ -match "^#define\s+(\w+)\s+([^/]+)") {$m[$Matches[1]]=$Matches[2].trim()}}
    end{$m}
}
filter Set-CDefine($m){
    $s=$_;while($s -match "\[[^A-Za-z]*([A-Za-z]\w+)"){$n=$Matches[1];$s=$s -replace $n,$m[$n]}
    if($s -match "\[([^\]]+)\]"){$n=$Matches[1];$s -replace ($n -replace "([-()*+])","`\`$1"),(Invoke-Expression $n)}else{$s}
}
filter Get-CStruct($types){begin{$old=$null}process{
    $a=((gc $_.pa -Encoding Oem)+(gc $_.pb -Encoding Oem))|Remove-CComment|Remove-CElse
    $b=$a|Get-CStructName
    $c=$a|Format-CStruct $b
	$v=$_.v
    $d=$c|%{$_ -replace "sizeof\([^*]+\*\)","4"}|%{$_ -replace "TTTSet\*","tttset_${v}*"}
    $e=$types|%{$f=$_;$d|?{$_ -match "^${f}"}}
    $f=$a|Get-CDefine $default_define
    $g=$e|Set-CDefine $f|%{$_ -replace "(enum|unsigned|struct)_","`$1 "}
    if (-not $old -or (Compare-Object $old $g)){@{v=$_.v;d=$g}}
    $old=$g
}}
filter Get-CStructCode{
    $v=$_.v
    $_.d|%{$st=$null}{
        $ss=$_.Split(" ",2)
        if ($st -ne $ss[0]){
            if ($st){"} ${st}_${v};";""}
            "typedef struct {"
        }
        $m="("+($subst_lst -join "|")+")"
        $s1=$ss[1] -replace "\."," " -replace $m,"`$1_${v}"
        if($s1 -match "\(\*\)"){
            $s2=$s1 -split " ";
            $s3=$s2[-1];
            $s1=($s2[0..($s2.count-2)] -join " ") -replace "\(\*\)","(*${s3})"
        }
        "    {0};" -f $s1
        $st=$ss[0]
    }{if ($st){"} ${st}_${v};";""}}
}
filter Get-CStructType($t){if($_ -match "}\s*(${t}_\w+)\s*;"){$Matches[1]}}
filter Get-CUnion($t){begin{"typedef union {"}process{
    if($_ -match "\d+"){$m=$Matches[0];"    ${_} v${m};"}
}end{"} u_${t};";""}
}

filter Get-CMacro($t,$nm){
    begin{$v1=0;$v2=0}
    process{if($_ -match "\d+"){$v2=$v1;$v1=$Matches[0];}}
    end{
        $s=if($v2){$t2=$t -replace "\d+",$v2;"(rv>=${v2})?${t2}(rv,p,mb):"}else{""}
        "#define ${t}(rv,p,mb) (${s}&(((u_${nm}*)(p))->v${v1}.##mb))";
    }
}

function make_source($src){
    "// generated {0}. " -f (Get-Date -Format "yyyy-MM-dd")
    @("","#pragma once","#include <windows.h>","")
    $src
    $type_lst|%{
        $t1=$_;$t2=$_.ToUpper()
        $m=$src|Get-CStructType $t1|sort -Descending
        $m|Get-CUnion $t1
        $m|%{$col=@();$tt=""}{$tt=$_.ToUpper();$col+=$_;$col|Get-CMacro $tt $t1}{
            $tt2=$tt -replace "\d+","0";"#define ${tt2}(rv,p,mb) ${tt}(rv,p,mb)";
        }
        "#define ${t2}_PTR(rv,p,mb,c) (${t2}_##c(rv,p,mb))";
        "#define ${t2}(rv,p,mb,c) (*${t2}_PTR(rv,p,mb,c))";"";
    }
}

## main program
$file_lst=ls ./_source/v*.h
$lst=$file_lst|%{$_.fullname}|Get-SourceVersion|sort -Unique|Get-SourceSet "./_source"
$col1=$lst|Get-CStruct $struct1_lst
$col2=$lst|Get-CStruct $struct2_lst
$src=($col1|Get-CStructCode)+($col2|Get-CStructCode)
make_source($src)|Out-File $out_file

##add version checker
$m=@{};($col1+$col2)|sort {$_.v} -Descending|%{$v=$_.v;$_.d|%{
    $ss=$_ -split " "
    $t=$ss[0].ToUpper()
    $n=$ss[-1] -replace "\[.*\]"
    $m[($t+"_"+"VER_"+$n)]=$v
}}
$m.Keys|%{"#define {0} {1}" -f $_,$m[$_]}|sort|Out-File $out_file -Append
""|Out-File $out_file -Append

$struct_lst|%{
    $t=$_.ToUpper()
    "#define TEST_${t}(v,mb) (v>=${t}_VER_##mb)"
}|Out-File $out_file -Append
""|Out-File $out_file -Append
