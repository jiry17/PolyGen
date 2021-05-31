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



(define-fun ans ((ldrop Int) (rdrop Int) (lzero Int2) (rzero Int2) (llen Int) (rlen Int)) Int
  (ite (= 1 lzero)
       (ite (= 1 rzero)
            (+ llen rlen)
            rdrop)
       ldrop))


(define-fun ib ((x Int)) Bool (and (>= x 0) (<= x 1)))


(declare-var a Int)
(declare-var b Int)
(declare-var c Int2)
(declare-var d Int2)
(declare-var e Int)
(declare-var f Int)
(constraint (=> (and (ib c) (ib d)) (= (ans a b c d e f) (combine a b c d e f))))

(check-synth)

