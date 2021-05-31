(set-logic LIA)

(synth-fun combine ((lans Int) (rans Int) (lmax Int) (rmax Int)) Int
    ((Start Int (lans
                 rans
                 lmax
                 rmax
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


(define-fun ans ((lans Int2) (rans Int2) (lmax Int) (rmax Int)) Int
  (ite (= 0 rans)
       0
       (ite (> lmax rmax)
            0
            1)))

(define-fun ib ((x Int)) Bool (and (>= x 0) (<= x 1)))

(declare-var a Int2)
(declare-var b Int2)
(declare-var c Int)
(declare-var d Int)
(constraint (=> (and (ib a) (ib b)) (= (ans a b c d) (combine a b c d))))

(check-synth)

