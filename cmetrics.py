# -*- coding: utf-8 -*-

# Copyright (C) 2007-2011 Israel Herraiz
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Library General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
#
# Authors : Israel Herraiz <isra@herraiz.org>


from commands import getoutput
import os
import argparse

MCCABE = "mccabe"
KDSI = "kdsi"
HALSTEAD = "halstead"

class SourceCodeFile:

    def __init__ (self, path):
        self.path = path
        self.name = os.path.split(path)[1]
        self.functions = []
        self.sloc = 0
        self.loc = 0
        self.blanks = 0
        self.comments = 0
        self.comment_l = 0
        self.hlength = 0
        self.hvolume = 0
        self.hlevel = 0
        self.hmd = 0
        self.nfuncs = 0

    def addFunction (self, func):
        func.parent_file = self
        self.functions.append(func)
        self.nfuncs = len([x for x in self.functions if not '***' in x.name])

    def getFunctions (self):

        return self.functions

    def getMaxCycloFunc (self):

        max_cyclo = 1

        candidate = None

        for x in self.functions:

            if x.cyclo >= max_cyclo:
                max_cyclo = x.cyclo
                candidate = x

        return candidate

    def getMinCycloFunc (self):
    
        min_cyclo = 1

        candidate = None

        for x in self.functions:

            if x.cyclo <= min_cyclo:
                min_cyclo = x.cyclo
                candidate = x

        return candidate

    def getAvgCyclo (self):

        cyclos = [x.cyclo for x in self.functions]

        return sum(cyclos) / len(cyclos)

    def getMedianCyclo (self):

        cyclos = [x.cyclo for x in self.functions]

        if len(cyclos) % 2 == 1:
            return cyclos[(len(cyclos)+1)/2-1]
        else:
            lower = cyclos[len(cyclos)/2-1]
            upper = cyclos[len(cyclos)/2]

        return (lower + upper) / 2  

    def getTotalCyclo (self):

        cyclos = [x.cyclo for x in self.functions]

        return sum(cyclos)
    
class Function:

    def __init__ (self, name, cyclo, sloc, returns):

        self.name = name
        self.parent_file = None
        self.sloc = int(sloc)
        self.cyclo = int(cyclo)
        self.returns = int(returns)


class CMetrics:

    def __init__ (self, mccabe_cmd, halstead_cmd, kdsi_cmd):

        self._files = []
        self._mccabe_cmd = mccabe_cmd
        self._halstead_cmd = halstead_cmd
        self._kdsi_cmd = kdsi_cmd
        
    def measure_target (self, target_dir):

        for root, dirs, files in os.walk(target_dir):
            for f in files:
                path = os.path.join(root, f)
                ext = os.path.splitext(path)[1]

                if '.c' in ext or '.h' in ext:
                    self._files.append (self.measure_file(path))
        
    def measure_file (self, sourcecode_fn):

        f = SourceCodeFile (sourcecode_fn)
    
        mccabe_o = getoutput("%s -n %s" % (self._mccabe_cmd, f.path))

        for l in mccabe_o.split('\n'):

            _name, func, sloc, cyclo, returns = l.split('\t')

            f.addFunction (Function (func, cyclo, sloc, returns))
            
        kdsi_o = getoutput("%s %s" % (self._kdsi_cmd, f.path))

        loc, blanks, comment_l, comments, _name = kdsi_o.split()
        f.loc = int(loc)
        f.blanks = int(blanks)
        f.comment_l = int(comment_l)
        f.comments = int(comments)
        f.sloc = f.loc - f.blanks - f.comment_l

        halstead_o = getoutput("%s %s" % (self._halstead_cmd, f.path))

        _name, hlength, hvolume, hlevel, hmd = halstead_o.split()
        f.hlength = int(hlength)
        f.hvolume = int(hvolume)
        f.hlevel = float(hlevel)
        f.hmd = int(hmd)
        
        return f

    def print_files_without_functions (self, show_path=False, show_header=True):

        if show_header:
            template = "{0:<12}  {1:>5}  {2:>5}  {3:>5}  {4:>5}  {5:>5}  {6:>5}  {7:>5}  {8:>5}  {9:>9}  {10:>9}  {11:>5}  {12:>5}  {13:>5}  {14:>5}  {15:>5}"
            print template.format("FILE", "SLOC", "LOC", "FUNCS", "BLANK", "COM.L", "COM.N", "H LEN", "H VOL", "H LEVEL", "H MEN. D.",  "MAXCY",  "MINCY", "AVGCY", "MEDCY", "TOTCY",)

        if show_path:
            template = "{0:<12}  {1:>5}  {2:>5}  {3:>5}  {4:>5}  {5:>5}  {6:>5}  {7:>5}  {8:>5}  {9:>9.3e}  {10:>9.3e}  {11:>5}  {12:>5}  {13:>5}  {14:>5}  {15:>5} {16:<}"
        else:
            template = "{0:<12}  {1:>5}  {2:>5}  {3:>5}  {4:>5}  {5:>5}  {6:>5}  {7:>5}  {8:>5}  {9:>9.3e}  {10:>9.3e}  {11:>5}  {12:>5}  {13:>5}  {14:>5}  {15:>5}"

        for f in self._files:

            max_cyclo_func = f.getMaxCycloFunc()
            if max_cyclo_func:
                max_cyclo = max_cyclo_func.cyclo
            else:
                max_cyclo = 1

            min_cyclo_func = f.getMinCycloFunc()
            if min_cyclo_func:
                min_cyclo = min_cyclo_func.cyclo
            else:
                min_cyclo = 1
                

            if show_path:
                print template.format(f.name,
                                  f.sloc,
                                  f.loc,
                                  f.nfuncs,
                                  f.blanks,
                                  f.comment_l,
                                  f.comments,
                                  f.hlength,
                                  f.hvolume,
                                  f.hlevel,
                                  f.hmd,
                                  max_cyclo,
                                  min_cyclo,
                                  f.getAvgCyclo(),
                                  f.getMedianCyclo(),
                                  f.getTotalCyclo(),
                                  f.path)
                            
            else:
                print template.format(f.name,
                                  f.sloc,
                                  f.loc,
                                  f.nfuncs,
                                  f.blanks,
                                  f.comment_l,
                                  f.comments,
                                  f.hlength,
                                  f.hvolume,
                                  f.hlevel,
                                  f.hmd,
                                  max_cyclo,
                                  min_cyclo,
                                  f.getAvgCyclo(),
                                  f.getMedianCyclo(),
                                  f.getTotalCyclo())


                
    def print_files_with_functions (self, show_path=False, show_header=True):

        if show_header:
            template = "{0:<12}  {1:<15}  {2:>5}  {3:>5}  {4:>5}"
            print template.format("FILE", "FUNC", "SLOC", "CYCLO", "RETURN",)

        if show_path:
            template = "{0:<12}  {1:<15}  {2:>5}  {3:>5}  {4:>5}  {5:<}"
        else:
            template = "{0:<12}  {1:<15}  {2:>5}  {3:>5}  {4:>5}"

        for f in self._files:
            for fun in f.functions:                
                if show_path:
                    print template.format(f.name[0:10],
                                      fun.name.split()[-1][0:15],
                                      fun.sloc,
                                      fun.cyclo,
                                      fun.returns,
                                      f.path)
                else:
                    print template.format(f.name[0:10],
                                      fun.name.split()[-1][0:15],
                                      fun.sloc,
                                      fun.cyclo,
                                      fun.returns)
                    
                    
if __name__ == '__main__':


    parser = argparse.ArgumentParser(description='Measure size and complexity metrics for C source code files')

    parser.add_argument('target_dir', nargs=1,
                        help='directory with sources (recursively scanned)')

    parser.add_argument('-f', '--functions', action='store_true',
                        help='show metrics for functions instead of files')

    parser.add_argument('-p', '--path', action='store_true',
                        help='show full file path in the last column')
    
    parser.add_argument('-n', '--no-header', action='store_false',
                        help='do not show metrics names in the first row')
    
    args = parser.parse_args()

    c = CMetrics(MCCABE, HALSTEAD, KDSI)
    c.measure_target(args.target_dir[0])

    header = args.no_header
    path = args.path
    
    if args.functions:
        c.print_files_with_functions(show_path=path, show_header=header)
    else:
        c.print_files_without_functions(show_path=path, show_header=header)
