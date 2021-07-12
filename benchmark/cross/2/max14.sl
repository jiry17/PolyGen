(set-logic LIA)


(synth-fun max14 ((x1 Int) (x2 Int) (x3 Int) (x4 Int) (x5 Int)
                 (x6 Int) (x7 Int) (x8 Int) (x9 Int) (x10 Int)
                 (x11 Int) (x12 Int) (x13 Int) (x14 Int)) Int
    ((Start Int (x1
                 x2
                 x3
                 x4
                 x5
                 x6
                 x7
                 x8
                 x9
                 x10
                 x11
                 x12
                 x13
                 x14
                 0
                 1
                 (+ Start Start)
                 (- Start Start)
                 (ite StartBool Start Start)))
     (StartBool Bool ((and StartBool StartBool)
                      (or  StartBool StartBool)
                      (not StartBool)
                      (<=  Start Start)
                      (=   Start Start)
                      (>=  Start Start)))))

(declare-var x1 Int)
(declare-var x2 Int)
(declare-var x3 Int)
(declare-var x4 Int)
(declare-var x5 Int)
(declare-var x6 Int)
(declare-var x7 Int)
(declare-var x8 Int)
(declare-var x9 Int)
(declare-var x10 Int)
(declare-var x11 Int)
(declare-var x12 Int)
(declare-var x13 Int)
(declare-var x14 Int)



(constraint (>= (max14 x1 x2 x3 x4 x5 x6 x7 x8 x9 x10 x11 x12 x13 x14) x1))
(constraint (>= (max14 x1 x2 x3 x4 x5 x6 x7 x8 x9 x10 x11 x12 x13 x14) x2))
(constraint (>= (max14 x1 x2 x3 x4 x5 x6 x7 x8 x9 x10 x11 x12 x13 x14) x3))
(constraint (>= (max14 x1 x2 x3 x4 x5 x6 x7 x8 x9 x10 x11 x12 x13 x14) x4))
(constraint (>= (max14 x1 x2 x3 x4 x5 x6 x7 x8 x9 x10 x11 x12 x13 x14) x5))
(constraint (>= (max14 x1 x2 x3 x4 x5 x6 x7 x8 x9 x10 x11 x12 x13 x14) x6))
(constraint (>= (max14 x1 x2 x3 x4 x5 x6 x7 x8 x9 x10 x11 x12 x13 x14) x7))
(constraint (>= (max14 x1 x2 x3 x4 x5 x6 x7 x8 x9 x10 x11 x12 x13 x14) x8))
(constraint (>= (max14 x1 x2 x3 x4 x5 x6 x7 x8 x9 x10 x11 x12 x13 x14) x9))
(constraint (>= (max14 x1 x2 x3 x4 x5 x6 x7 x8 x9 x10 x11 x12 x13 x14) x10))
(constraint (>= (max14 x1 x2 x3 x4 x5 x6 x7 x8 x9 x10 x11 x12 x13 x14) x11))
(constraint (>= (max14 x1 x2 x3 x4 x5 x6 x7 x8 x9 x10 x11 x12 x13 x14) x12))
(constraint (>= (max14 x1 x2 x3 x4 x5 x6 x7 x8 x9 x10 x11 x12 x13 x14) x13))
(constraint (>= (max14 x1 x2 x3 x4 x5 x6 x7 x8 x9 x10 x11 x12 x13 x14) x14))


(constraint (or (= x1 (max14 x1 x2 x3 x4 x5 x6 x7 x8 x9 x10 x11 x12 x13 x14))
            (or (= x2 (max14 x1 x2 x3 x4 x5 x6 x7 x8 x9 x10 x11 x12 x13 x14))
            (or (= x3 (max14 x1 x2 x3 x4 x5 x6 x7 x8 x9 x10 x11 x12 x13 x14))
            (or (= x4 (max14 x1 x2 x3 x4 x5 x6 x7 x8 x9 x10 x11 x12 x13 x14))
            (or (= x5 (max14 x1 x2 x3 x4 x5 x6 x7 x8 x9 x10 x11 x12 x13 x14))
            (or (= x6 (max14 x1 x2 x3 x4 x5 x6 x7 x8 x9 x10 x11 x12 x13 x14))
            (or (= x7 (max14 x1 x2 x3 x4 x5 x6 x7 x8 x9 x10 x11 x12 x13 x14))
            (or (= x8 (max14 x1 x2 x3 x4 x5 x6 x7 x8 x9 x10 x11 x12 x13 x14))
            (or (= x9 (max14 x1 x2 x3 x4 x5 x6 x7 x8 x9 x10 x11 x12 x13 x14))
            (or (= x10 (max14 x1 x2 x3 x4 x5 x6 x7 x8 x9 x10 x11 x12 x13 x14))
            (or (= x11 (max14 x1 x2 x3 x4 x5 x6 x7 x8 x9 x10 x11 x12 x13 x14))
            (or (= x12 (max14 x1 x2 x3 x4 x5 x6 x7 x8 x9 x10 x11 x12 x13 x14))
            (or (= x13 (max14 x1 x2 x3 x4 x5 x6 x7 x8 x9 x10 x11 x12 x13 x14))
                (= x14 (max14 x1 x2 x3 x4 x5 x6 x7 x8 x9 x10 x11 x12 x13 x14))))))))))))))))

(check-synth)

(define-fun res ((Param0 Int) (Param1 Int) (Param2 Int) (Param3 Int) (Param4 Int) (Param5 Int) (Param6 Int) (Param7 Int) (Param8 Int) (Param9 Int) (Param10 Int) (Param11 Int) (Param12 Int) (Param13 Int)) Int (ite (and (and (and (and (and (and (and (and (and (and (and (and (< Param13 Param0) (< Param12 Param0)) (< Param11 Param0)) (< Param10 Param0)) (< Param9 Param0)) (< Param8 Param0)) (< Param7 Param0)) (< Param6 Param0)) (< Param5 Param0)) (< Param4 Param0)) (< Param3 Param0)) (< Param2 Param0)) (< Param1 Param0)) Param0 (ite (and (and (and (and (and (and (and (and (and (and (and (< Param13 Param1) (< Param12 Param1)) (< Param11 Param1)) (< Param10 Param1)) (< Param9 Param1)) (< Param8 Param1)) (< Param7 Param1)) (< Param6 Param1)) (< Param5 Param1)) (< Param4 Param1)) (< Param3 Param1)) (< Param2 Param1)) Param1 (ite (and (and (and (and (and (and (and (and (and (and (< Param13 Param2) (< Param12 Param2)) (< Param11 Param2)) (< Param10 Param2)) (< Param9 Param2)) (< Param8 Param2)) (< Param7 Param2)) (< Param6 Param2)) (< Param5 Param2)) (< Param4 Param2)) (< Param3 Param2)) Param2 (ite (and (and (and (and (and (and (and (and (and (< Param13 Param3) (< Param12 Param3)) (< Param11 Param3)) (< Param10 Param3)) (< Param9 Param3)) (< Param8 Param3)) (< Param7 Param3)) (< Param6 Param3)) (< Param5 Param3)) (< Param4 Param3)) Param3 (ite (and (and (and (and (and (and (and (and (< Param13 Param4) (< Param12 Param4)) (< Param11 Param4)) (< Param10 Param4)) (< Param9 Param4)) (< Param8 Param4)) (< Param7 Param4)) (< Param6 Param4)) (< Param5 Param4)) Param4 (ite (and (and (and (and (and (and (and (< Param13 Param5) (< Param12 Param5)) (< Param11 Param5)) (< Param10 Param5)) (< Param9 Param5)) (< Param8 Param5)) (< Param7 Param5)) (< Param6 Param5)) Param5 (ite (and (and (and (and (and (and (< Param13 Param6) (< Param12 Param6)) (< Param11 Param6)) (< Param10 Param6)) (< Param9 Param6)) (< Param8 Param6)) (< Param7 Param6)) Param6 (ite (and (and (and (and (and (< Param13 Param7) (< Param12 Param7)) (< Param11 Param7)) (< Param10 Param7)) (< Param9 Param7)) (< Param8 Param7)) Param7 (ite (and (and (and (and (< Param13 Param8) (< Param12 Param8)) (< Param11 Param8)) (< Param10 Param8)) (< Param9 Param8)) Param8 (ite (and (and (and (< Param13 Param9) (< Param12 Param9)) (< Param11 Param9)) (< Param10 Param9)) Param9 (ite (and (and (< Param13 Param10) (< Param12 Param10)) (< Param11 Param10)) Param10 (ite (and (< Param13 Param11) (<= Param12 Param11)) Param11 (ite (<= Param13 Param12) Param12 Param13))))))))))))))