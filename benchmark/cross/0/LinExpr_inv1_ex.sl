(set-logic LIA)

(synth-fun inv ((x Int) (y Int)) Int
    ((Start Int (x
                 y
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




(declare-var x Int)
(declare-var y Int)

(constraint (= (inv x y) (ite (or (= x 1) (= y 1)) 3 1)))

(check-synth) 
(define-fun res ((Param0 Int) (Param1 Int)) Int (ite (or (and (<= 1 Param0) (<= Param0 1)) (and (<= 1 Param1) (<= Param1 1))) 3 1))