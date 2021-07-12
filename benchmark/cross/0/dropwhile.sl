(set-logic LIA)

(synth-fun combine ((ldrop Int) (rdrop Int) (lzero Int) (rzero Int) (llen Int) (rlen Int)) Int
    ((Start Int (ldrop
                 rdrop
                 lzero
                 rzero
                 llen
                 rlen
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



(define-fun ans ((ldrop Int) (rdrop Int) (lzero Int) (rzero Int) (llen Int) (rlen Int)) Int
  (ite (= 1 lzero)
       (ite (= 1 rzero)
            (+ llen rlen)
            rdrop)
       ldrop))


(define-fun ib ((x Int)) Bool (and (>= x 0) (<= x 1)))


(declare-var a Int)
(declare-var b Int)
(declare-var c Int)
(declare-var d Int)
(declare-var e Int)
(declare-var f Int)
(constraint (=> (and (ib c) (ib d)) (= (ans a b c d e f) (combine a b c d e f))))

(check-synth)

(define-fun res ((Param0 Int) (Param1 Int) (Param2 Int) (Param3 Int) (Param4 Int) (Param5 Int)) Int (ite (<= Param2 0) Param0 (ite (<= Param3 0) Param1 (+ Param4 Param5))))