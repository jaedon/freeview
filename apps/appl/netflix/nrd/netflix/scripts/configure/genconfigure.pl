#!/usr/bin/perl

use Cwd;
use Cwd 'abs_path';
use File::Basename;
use File::Spec;

use strict;

my %flags;
my %groups;
my $help_columns = 35;
my $project_name;

sub generateOption {
    my $result = $_[0];
    $result =~ s,^BUILD_,,;
    $result =~ s,^CMAKE_,,;
    $result =~ s,_,-,g;
    $result = lc($result);
    return $result;
}

sub processCMake {
    my ($file) = @_;
    print "Processing: $file\n";
    if(open(FILE, "<$file")) {
        my $parsable;
        while(my $line = <FILE>) {
            chomp($line);
            $line =~ s,\s*#.*$,,;
            if($line =~ /\(/ && !($line =~ /\)/)) {
                $parsable .= $line;
                next;
            } else {
                $parsable = $line;
            }

            #print "See: $parsable\n";
            my %flag;
            if($parsable =~ /^\s*option\s*\(\s*([\S]+)\s*"([^"]*)"\s*((?i:ON)|(?i:OFF))\s*\)/) {
                $flag{cmake} = "$1";
                $flag{type} = "BOOL";
                $flag{desc} = "$2";
                $flag{default} = "$3";
            } elsif($parsable =~ /^\s*get_environment_variable\s*\(\s*([\S]+)\s*"([^"]*)"\s*"([^"]*)"\s*\)/) {
                $flag{cmake} = "$1";
                $flag{type} = "ENV";
                $flag{desc} = "$2";
                $flag{default} = "$3";
            } elsif($parsable =~ /^\s*add_component_option\s*\(\s*([\S]+)\s*"([^"]*)"\s*((?i:ON)|(?i:OFF))\s*([\S]+)\s*\)/) {
                $flag{cmake} = "$1";
                $flag{type} = "BOOL";
                $flag{desc} = "$2";
                $flag{default} = "$3";
            } elsif($parsable =~ /^\s*set\s*\(\s*([\S]+)\s*"([^"]*)"\s*(?i:CACHE)?\s*([\S]+)\s*"([^"]*)"\s*(?i:FORCE)?\s*\)/) {
                $flag{cmake} = "$1";
                $flag{type} = "$3";
                $flag{desc} = "$4";
                $flag{default} = "$2";
            } elsif($parsable =~ /^\s*set\s*\(\s*GROUP_([\S]+)\s*"([^"]*)"\s*\)/) {
                my $group = $1;
                my $desc = $2;
                print "Found group: $group\n";
                $groups{$group} = $desc;
            } elsif($parsable =~ /^\s*project\s*\(([^\)]*)\)/) {
                $project_name = $1;
                if ($project_name =~ m/([^\s]+)/) {
                    $project_name = $1;
                }

                print "Found project: $project_name\n";
            }
            if(defined(%flag)) {
                foreach(keys(%groups)) {
                    my $group = $_;
                    if($flag{cmake} =~ /^$group/) {
                        $flag{group} = $group;
                        last;
                    }
                }
                $flag{option} = generateOption($flag{cmake}) unless(defined($flag{option}));
                my @aliases;
                $flag{desc} =~ s,','\\'',g;
                if($flag{desc} =~ /^\[([^\]]*)\]\s*/) {
                    my $modifiers = "$1";
                    $flag{desc} =~ s,^\[\Q${modifiers}\E\]\s*,,;
                    foreach(split(/:/, $modifiers)) {
                        my $modifier = $_;
                        if($modifier =~ /^-/) {
                            $flag{option} = substr($modifier, 1);
                        } elsif($modifier =~ /^\+/) {
                            push @aliases, substr($modifier, 1);
                        } elsif($modifier eq "LIST" || $modifier eq "COMPONENT" || $modifier eq "HIDDEN") {
                            $flag{type} = $modifier;
                        }
                    }
                }
                print "Found option($project_name): " . $flag{cmake} . "(" . $flag{option} . ") [" . $flag{type} . "]\n";
                $flags{$flag{option}} = \%flag;
                foreach(@aliases) {
                    my $alias = $_;
                    my %alias_flag = %flag;
                    $alias_flag{option} = $alias;
                    delete $alias_flag{desc};
                    print "  Alias: " . $alias_flag{cmake} . "(" . $alias_flag{option} . ") [" . $alias_flag{type} . "]\n";
                    $flags{$alias_flag{option}} = \%alias_flag;
                }
            }
        }
        close(FILE);
    }
}

sub processDirectory {
    my ($dir) = @_;
    my @subdirs;
    my $old_project_name = $project_name;
    if(opendir(DIR, $dir) ) {
        while(my $file = readdir(DIR)) {
            next if($file eq "." || $file eq ".." || $file eq "macros.cmake" || $file eq "3rdparty");
            if(-d "$dir/$file") {
                push @subdirs, $file;
            } elsif($file eq "CMakeLists.txt" || $file =~ /\.cmake$/) {
                processCMake("$dir/$file");
            }
        }
        closedir(DIR);
        foreach(@subdirs) {
            if(-e "$dir/$_/.configure.helper") {
                print "Skipping: $dir/$_\n";
            } else {
                processDirectory("$dir/$_");
            }
        }
    }
    $project_name = $old_project_name if($old_project_name);
}

my @dirs;
my $cwd = Cwd::cwd();
if($#ARGV == -1) {
    push @dirs, $cwd;
} else {
    foreach(@ARGV) {
        push @dirs, $_;
    }
}
foreach(@dirs) {
    my $dir = $_;
    print "Processing Root: $dir\n";
    processDirectory($dir);
}

$project_name = "main" unless($project_name);
print "Generating project: $project_name\n";
if(open(HELPER, ">$cwd/.configure.helper")) {
    #write the help options
    print HELPER "PROJECTS=\"\$PROJECTS $project_name\"\n";
    print HELPER "configHelper_${project_name}_helpOptions() {\n  true\n";
    my @groups = sort keys(%groups);
    unshift( @groups, "");
    foreach(@groups) {
        my $group = $_;
        print HELPER "  echo '\n" . $groups{$group} . ":\n'\n  " if(length($group));
        my @group_flags;
        foreach(keys(%flags)) {
            my %flag = %{$flags{$_}};
            push @group_flags, \%flag if($flag{group} eq $group);
        }
        foreach(sort { $a->{option} cmp $b->{option} } @group_flags) {
            my %flag = %{$_};
            next if(!$flag{desc});
            my $option = $flag{option};
            my $desc = $flag{desc};
            my $help1;
            my $help2;
            my $help_default = $flag{default};
            if($flag{type} eq "BOOL") {
                $help1 = "--${option}";
                $help2 = "--no-${option}";
                $help_default = ($help_default =~ /on/i) ? "enabled" : "disabled";
            } elsif($flag{type} eq "STRING" || $flag{type} eq "LIST" || $flag{type} eq "PATH" || $flag{type} eq "COMPONENT") {
                $help1 = "--${option}=value";
                $help_default =~ s/;/,/g if($flag{type} eq "LIST");
                $help_default = "\"$help_default\""
            }
            if(defined($help1) || defined($help2)) {
                print HELPER "  echo '  $help1";
                for(my $i = length($help1); $i < $help_columns; ++$i) {
                    print HELPER " ";
                }

                require Text::Format;
                import Text::Format;
                my $text = Text::Format->new;
                $text->rightFill(0);
                $text->firstIndent(0);
                $text->columns(80);
                $text->bodyIndent($help_columns+2);
                print HELPER $text->format("${desc}'\n");

                print HELPER "  echo '  $help2";
                for(my $i = length($help2); $i < $help_columns; ++$i) {
                    print HELPER " ";
                }
                print HELPER "[default=${help_default}]'\n";
                print HELPER "  echo\n";
            }
        }
    }
    print HELPER "}\n\n";
    #write the help env
    print HELPER "configHelper_${project_name}_helpEnv() {\n  true\n";
    foreach(keys(%flags)) {
        my %flag = %{$flags{$_}};
        if($flag{type} eq "ENV" && $flag{desc}) {
            my $help = $flag{cmake};
            print HELPER "  echo '  $help";
            for(my $i = length($help); $i < $help_columns; ++$i) {
                print HELPER " ";
            }
            print HELPER $flag{desc} . "'\n";
            print HELPER "  echo '  ";
            for(my $i = 0; $i < $help_columns; ++$i) {
                print HELPER " ";
            }
            if($flag{default}) {
                print HELPER "[default=" . $flag{default} . "]\n'\n";
            } else {
                print HELPER "'\n";
            }
        }
    }
    print HELPER "}\n\n";
    #write the parse option
    print HELPER "configHelper_${project_name}_parseOption() {\n  OPT=\"\$1\"; case \"\$OPT\" in\n";
    foreach(keys(%flags)) {
        my %flag = %{$flags{$_}};
        my $option = $flag{option};
        my $desc = $flag{desc};
        my $cmake = $flag{cmake};
        my $default = $flag{default};
        if($flag{type} eq "BOOL") {
            print HELPER "  --enable-${option}|--${option}) CMAKE_FLAGS=\"\$CMAKE_FLAGS -D${cmake}=1\"; cmdFlags[${option}]=1; defaults[${option}]=\"${default}\" ;;\n";
            print HELPER "  --no-${option}|--disable-${option}) CMAKE_FLAGS=\"\$CMAKE_FLAGS -D${cmake}=0\"; cmdFlags[${option}]=0; defaults[${option}]=\"${default}\" ;;\n";
            print HELPER "  --${option}=*) CMAKE_FLAGS=\"\$CMAKE_FLAGS -D${cmake}=`echo \$OPT | sed 's,^--${option}=,,'`\"; cmdFlags[${option}]=`echo \$OPT | sed 's,^--${option}=,,'`; defaults[${option}]=\"${default}\" ;;\n";
        } elsif($flag{type} eq "STRING" || $flag{type} eq "LIST" || $flag{type} eq "PATH" || $flag{type} eq "COMPONENT") {
            my $transform = "sed -e 's,--.*=\\(.*\\),\\1,g'";
            $transform .= " -e 's/,/;/g'" if($flag{type} eq "LIST");
            print HELPER "  --with-${option}=*|--${option}=*) opt=`echo \$OPT | $transform`; CMAKE_FLAGS=\"\$CMAKE_FLAGS -D${cmake}=\\\"\${opt}\\\"\"";
            print HELPER "; components[${option}]=\"\${opt}\"; processComponentHelper \"\${opt}\"" if($flag{type} eq "COMPONENT");
            print HELPER "; cmdFlags[${option}]=\"\${opt}\"" if($flag{type} ne "COMPONENT");
            print HELPER ";;\n";
        }
    }
    print HELPER "  *) return 1;;\n";
    print HELPER "  esac;return 0;\n}\n";
    #default components
    foreach(keys(%flags)) {
        my %flag = %{$flags{$_}};
        if($flag{type} eq "COMPONENT") {
            print HELPER "processComponentHelper \"$flag{option}\"\n";
        }
    }
    close(HELPER);
}
