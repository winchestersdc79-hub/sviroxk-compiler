; ModuleID = 'sviroxk'
source_filename = "sviroxk"

@0 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1
@1 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1

define i32 @main() {
entry:
  %Player_health = alloca i32, align 4
  store i32 100, ptr %Player_health, align 4
  %Player_speed = alloca i32, align 4
  store i32 5, ptr %Player_speed, align 4
  %0 = load i32, ptr %Player_health, align 4
  %1 = call i32 (ptr, ...) @printf(ptr @0, i32 %0)
  %2 = load i32, ptr %Player_speed, align 4
  %3 = call i32 (ptr, ...) @printf(ptr @1, i32 %2)
  ret i32 0
}

declare i32 @puts(ptr)

declare i32 @printf(ptr, ...)
