; max5.sl
; Synthesize the maximum of 5 integers, from a purely declarative spec

(set-logic LIA)

(synth-fun max9 ((x1 Int) (x2 Int) (x3 Int) (x4 Int) (x5 Int)
                 (x6 Int) (x7 Int) (x8 Int) (x9 Int)) Int
    ((Start Int (x1
                 x2
                 x3
                 x4
                 x5
                 x6 
                 x7
                 x8
                 x9
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

(constraint (>= (max9 x1 x2 x3 x4 x5 x6 x7 x8 x9) x1))
(constraint (>= (max9 x1 x2 x3 x4 x5 x6 x7 x8 x9) x2))
(constraint (>= (max9 x1 x2 x3 x4 x5 x6 x7 x8 x9) x3))
(constraint (>= (max9 x1 x2 x3 x4 x5 x6 x7 x8 x9) x4))
(constraint (>= (max9 x1 x2 x3 x4 x5 x6 x7 x8 x9) x5))
(constraint (>= (max9 x1 x2 x3 x4 x5 x6 x7 x8 x9) x6))
(constraint (>= (max9 x1 x2 x3 x4 x5 x6 x7 x8 x9) x7))
(constraint (>= (max9 x1 x2 x3 x4 x5 x6 x7 x8 x9) x8))
(constraint (>= (max9 x1 x2 x3 x4 x5 x6 x7 x8 x9) x9))


(constraint (or (= x1 (max9 x1 x2 x3 x4 x5 x6 x7 x8 x9))
            (or (= x2 (max9 x1 x2 x3 x4 x5 x6 x7 x8 x9))
            (or (= x3 (max9 x1 x2 x3 x4 x5 x6 x7 x8 x9))
            (or (= x4 (max9 x1 x2 x3 x4 x5 x6 x7 x8 x9))
            (or (= x5 (max9 x1 x2 x3 x4 x5 x6 x7 x8 x9))
            (or (= x6 (max9 x1 x2 x3 x4 x5 x6 x7 x8 x9))
            (or (= x7 (max9 x1 x2 x3 x4 x5 x6 x7 x8 x9))
            (or (= x8 (max9 x1 x2 x3 x4 x5 x6 x7 x8 x9))
	        (= x9 (max9 x1 x2 x3 x4 x5 x6 x7 x8 x9)))))))))))

(check-synth)


(define-fun res ((Param0 Int) (Param1 Int) (Param2 Int) (Param3 Int) (Param4 Int) (Param5 Int) (Param6 Int) (Param7 Int) (Param8 Int)) Int (ite (and (and (and (and (and (and (and (< Param8 Param0) (< Param7 Param0)) (< Param6 Param0)) (< Param5 Param0)) (< Param4 Param0)) (< Param3 Param0)) (< Param2 Param0)) (< Param1 Param0)) Param0 (ite (and (and (and (and (and (and (< Param8 Param1) (< Param7 Param1)) (< Param6 Param1)) (< Param4 Param1)) (< Param3 Param1)) (< Param2 Param1)) (<= Param5 Param1)) Param1 (ite (and (and (and (and (and (< Param8 Param2) (< Param7 Param2)) (< Param6 Param2)) (< Param5 Param2)) (< Param4 Param2)) (< Param3 Param2)) Param2 (ite (and (and (and (and (< Param8 Param3) (< Param7 Param3)) (< Param6 Param3)) (< Param5 Param3)) (< Param4 Param3)) Param3 (ite (and (and (and (< Param8 Param4) (< Param7 Param4)) (< Param6 Param4)) (< Param5 Param4)) Param4 (ite (and (and (< Param7 Param5) (< Param6 Param5)) (<= Param8 Param5)) Param5 (ite (and (<= Param8 Param6) (<= Param7 Param6)) Param6 (ite (<= Param8 Param7) Param7 Param8)))))))))