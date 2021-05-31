(set-logic LIA)

(synth-fun combine ((lans Int) (rans Int) (lmin Int) (rmin Int)) Int
    ((Start Int (lans
                 rans
                 lmin
                 rmin
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


(define-fun max2 ((x Int) (y Int)) Int 
  (ite (>= x y) x y)
)
(define-fun ans  ((lans Int) (rans Int) (lmin Int) (rmin Int)) Int
  (ite (< lans rmin)
       lans
       (ite (< rans lmin)
            rans
            (max2 lmin rmin))))


(declare-var a Int)
(declare-var b Int)
(declare-var c Int)
(declare-var d Int)
(constraint (= (ans a b c d) (combine a b c d)))

(check-synth)

