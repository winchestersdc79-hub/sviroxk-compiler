; ModuleID = 'sviroxk'
source_filename = "sviroxk"

@0 = private unnamed_addr constant [9 x i8] c"5 + 3 = \00", align 1

define i32 @main() {
entry:
  %0 = call i32 @puts(ptr @0)
  ret i32 0
}

declare i32 @puts(ptr)
