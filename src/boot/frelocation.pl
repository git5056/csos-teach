#use strict;
# my $output = `ls -l`;
# print "ls:$output";
my $output2 = `objdump.exe -x ../../build/kernel32.pe`;
# my $output2 = ` objdump.exe -xd ../../build/kernel32.pe`;
my @arr = split('\n', $output2);
# print "obj:@arr\n";
# print "obj:$#arr\n";
my @outcon1 = qw(); 
foreach my $element (@arr) {
    my $line = $element;
    chomp $line;
        $line =~ s/\t/ /g;
    $line =~ s/\s{2,}/ /g;
    $line =~ s/^\s//g;
    @outcon1 = (@outcon1,$line);
    # print "xx:" . $element . "\n";
}

my @arr2 = grep(/^\s*?\d+\s+\.\S+/,@outcon1);
# @arr2 = grep(/debug_info/,@arr2);
@arr2 = grep(/init_task/,@arr2);
# @arr2 = grep(/text/,@arr2);
my $addr = $arr2[0];
@arr2 = split('\s',$addr);
foreach my $element (@arr2) {
    my $line = $element;
    # print "xx:" . $element . "\n";
}
$addr = $arr2[3];
print "addr:$addr\n";
# $addr = 0 + $addr;
$sec_addr = hex( $addr);
$sec_len = hex($arr2[2]);
print "addr:$sec_addr\n";
my $aim = 0;
my $imagebase = hex('0x400000');
my @aimLines = qw();
foreach my $element (@outcon1) {
# Virtual Address: 00004000 Chunk size 76 (0x4c) Number of fixups 34
    if($element =~ /Virtual\s+Address\:\s+\w+\s+Chunk\s+size/){
        print "line:$element\n";
        my @tmp = split('\s',$element);
        my $tmpa =hex( $tmp[2]);
        my $addr = $imagebase + $tmpa;
        print "line:$addr $sec_addr $sec_len\n";
        print "line:$addr ($imagebase+$sec_addr) ($imagebase+$sec_len)\n";
        my $t1 = $imagebase + $sec_addr; 
        my $t2 = $imagebase + $sec_len; # error 
        print "line:$t1 $t2\n";
        if($sec_addr <= $addr && $addr<=($sec_addr+$sec_len)){
            $aim=1;
            print "line_$aim:$element\n";
            next;
        }
    }
    
            # print "qwe123line_$aim:$element\n";
    if($aim){
            print "123line_$aim:$element\n";
        # reloc    0 offset  116 [60116] HIGHLOW
    	# reloc    1 offset  11d [6011d] HIGHLOW
        if($element =~ /reloc/){
            print "xx line:$element\n";
            @aimLines = (@aimLines,$element);
        }else{
            $aim=0;
        }
    }
}
# while(my $line = @arr){
#     chomp $line;
#     print "line:$line\n";
# }
# print "obj:$output2";
# gccprint

# open(MYFILE,"init_task_relocation.txt") || die "$!";
@outcon=qw();
$quit_y="n";

foreach my $line (@aimLines) {
# while (my $line = <MYFILE>) {
    chomp $line;
    # print "xxline$line\n";
    # print "qweqwe\n";

    $line =~ s/\t/ /g;
    $line =~ s/\s{2,}/ /g;
    $line =~ s/^\s//g;
    # print "xxline$line\n";
    if($line =~ /\[([\da-f]+)\]\s+HIGHLOW$/){
        # print "xxline2:$1\n";
        print "$1\n";
        @outcon = (@outcon,$1);
    }
 
    next if(1);
    if($line =~ /\./){
        @arr = split(/ /,$line);
        
        # print " @arr \t$#arr \n";
        my $idx=0;
        foreach $item  (@arr){
            $idx++;
            # print "bbb  $idx\t$item\n";
        }

        # print "aaaaa  $arr[7] \t$#arr \n";
        print "\{ 0x$arr[5],  0x$arr[3]-0x400000,0x$arr[2]\}\, \n";
        #         {0x400,0x11000,0x00020d8a  },
        # {0x00021200 ,0x32000,0x00004ff2    },
    }

    next if(1);
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

# close(MYFILE);

$a = join("\n",@outcon);
open(MYFILE_OUT,">init_task_relocation_output.txt");
print MYFILE_OUT $a;
close MYFILE_OUT;
