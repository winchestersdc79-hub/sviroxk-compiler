; ModuleID = 'sviroxk'
source_filename = "sviroxk"

@0 = private unnamed_addr constant [13 x i8] c"\D0\B2\D0\BD\D1\83\D1\82\D1\80\D0\B8\00", align 1

define i32 @main() {
entry:
  %0 = call i32 @add()
  ret i32 0
}

declare i32 @puts(ptr)

declare i32 @printf(ptr, ...)

define i32 @add() {
entry:
  %0 = call i32 @puts(ptr @0)
  ret i32 0
}
