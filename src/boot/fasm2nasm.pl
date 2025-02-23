#use strict;
$a = "q";
$b = "w";
$c = $a. $b;
print "$c\n";
$line=1;
open(MYFILE,"src\\boot\\a.tmp.c") || die "$!";
@outcon=qw();
$quit_y="n";
while (my $line = <MYFILE>) {
    chomp $line;
    #去除如506:	55                   	push   %ebp中的506:
    # $line =~ s/^(\d|a-z)+/\t/; 
    $line =~ s/\%//g;
    $line =~ s/\$//g;
    $line =~ s/\t/ /g;
    if ($line =~ /\,/){
        if ($line =~ /(\w+)\s+(\S+),\s*(\S+)/) {
            my ($op, $val, $reg) = ($1, $2, $3);
            $line = "\t$op $reg ,$val";# chchchch $line";
        }
    }

    # 处理 -0x12(ebp)或+0x12(ebp)
    if ($line =~ /([\-\+].+)(\(.+)(\))/){
        # print "1:$1\n";
        # print "2:$2\n";
        # print "3:$3\n";
        $line =~ s/([\-\+].+)(\(.+)(\))/$2$1$3/g;
        $line =~ s/\(/\[/g;
        $line =~ s/\)/\]/g;
    }

    #处理 ,0x12(ebp)
    if ($line =~ /(?<=\,)(\S+)(\(.+?)(\))/){
        # print "1:$1\n";
        # print "2:$2\n";
        # print "3:$3\n";
        # print "4:$4\n";
        # print "line:$line\n";
        $line =~ s/(?<=\,)(\S+)(\(.+?)(\))/$2\+$1$3/g;
        $line =~ s/\(/\[/g;
        $line =~ s/\)/\]/g;
        #处理如 mov edx [ebp+,]
        # print "line2:$line\n";
    }

    #处理 ,(ebp)
    if ($line =~ /(?<=\,)(\s*?)(\(.+)(\))/){
        # print "1:$1\n";
        # print "2:$2\n";
        # print "3:$3\n";
        # print "4:$4\n";
        # print "line:$line\n";
        $line =~ s/(?<=\,)(\s*?)(\(.+)(\))/$2$3/g;
        $line =~ s/\(/\[/g;
        $line =~ s/\)/\]/g;
        # print "line2:$line\n";
    }

    $line =~ s/\t/ /;
    # $line =~ s/ /tmpxxtmp/g;
    # if ($line =~ /(?<=tmpxxtmp)([^[tmpxxtmp]]*?)(\(.+)(\))/){
     if ($line =~ /(?<=\s)([^\s\,]*?)(\(.+)(\))/){
        # print "1:$1\n";
        # print "2:$2\n";
        # print "3:$3\n";
        # print "4:$4\n";
        # print "line:$line\n";
        if($1 =~ /^\s*$/){
            $line =~ s/(?<=\s)([^\s\,]*?)(\(.+)(\))/$2$1$3$4/g;
        }else{
            $line =~ s/(?<=\s)([^\s\,]*?)(\(.+)(\))/$2\+$1$3$4/g;
        }
      
        $line =~ s/\(/\[/g;
        $line =~ s/\)/\]/g;
        # print "line2:$line\n";
    }
    # $line =~ s/tmpxxtmp/ /g;
    
    if($quit_y ne "q"){
        print "wait quit_y:$quit_y\n";
        $quit_y=<STDIN>;
        chomp $quit_y;
    }

    # if ($line =~ /\,/){
    #      @t3 = grep /(\s.+?)\,(.+?)$/,$line;
    #      foreach $b (@t3){
    #         print "1:$b\n"
    #      }
    #     print "t3t3t3t3:@t3\tzzzz:$1\n"
    # }

    $line =~ s/movl/mov dword/g;
    $line =~ s/cmpl/cmp dword/g;
    $line =~ s/incl/inc dword/g;
    # movzbl (123).%ecx==> movzx ecx, byte [123]
    if($line =~ /movzbl/){
        $line =~ s/(\S+?)\s(.*?\,)/$1 dword $2/;
        $line =~ s/(\,)(.+)$/$1 byte $2/;
        $line =~ s/movzbl/movzx/;
        # print "1:$1\n";
        # print "2:$2\n";
        # print "3:$3\n";
        # print "4:$4\n";
        # print "line:$line\n";
    }

    # 如jmp    780 <_memory_check+0x28>   ===>jmp _memory_check+0x28 ;   780 <_memory_check+0x28> 
    if($line =~ /je|jbe|jmp/){
        if($line =~ /\<(.+)\>/){
            $tmp1=$1;
            $line =~ s/(\S+?)\s(.*)/$1 $tmp1 \;$2/;
        }
    }

    print "$line\n";
    @outcon = (@outcon,$line);
}

close(MYFILE);
print "@outcon  qqq\n";
$a = join("\n",@outcon);
# print "@a  qqq\n";

open(MYFILE_OUT,">src\\boot\\out.fasm");
print MYFILE_OUT $a;
@aa =qw(;123 123 235);
print MYFILE_OUT @aa;
close MYFILE_OUT;
my $code = 'add 0x200,esp';
if ($code =~ /(\w+)\s+(\S+),\s*(\S+)/) {
    my ($op, $val, $reg) = ($1, $2, $3);
    $code = "$op $reg ,$val ";  # 重新排列为寄存器在前，数值在后，操作符在最后
}
#print $code;  # 输出：esp, 0x200, sub （注意这里使用了'sub'而不是'add'，根据你的需求可以调整）


