You should use the HW4 code repo for HW5. However, there are a few additions to deal with HW5 requirements (for array and class). Here is an explanation of what files are changed for the purpose of HW5. You may want to diff the files to see the difference. THese changes should not break your HW4 code.

Under include/ir

1. temp.hh: added a new string label type.
2. treep.hh: added string label into the Name type as a member.
3. Tr_exp: added Kind information and the getKind in each of the Tr_ex, Tr_nx, Tr_cx types (to make it bit eaiser to tell the type of the Tr_exp)

Under lib/ir

1. tree2xml.cc: to accommodate the changes in treep.hh