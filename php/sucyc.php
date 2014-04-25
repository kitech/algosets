<?php

// super little cycle

// 1.8-1.9
function sucyc_v1() {
    for ($i = 0; $i <= 100; ++ $i) {
        if ($i % 3 == 0 && $i % 5 == 0) {
            // echo "*#";
        } else if ($i % 3 == 0) {
            // echo "*";
        } else if ($i % 5 == 0) {
            // echo "#";
        } else {
        }
    }

    // echo "\n";
}

// 1.5 - 1.6
function sucyc_v2() {
    for ($i = 0; $i <= 100; ++ $i) {
        if ($i % 15 == 0) {
            // echo "*#";
        } else if ($i % 3 == 0) {
            // echo "*";
        } else if ($i % 5 == 0) {
            // echo "#";
        } else {
        }
    }

    // echo "\n";
}

// 4.2 - 4.3
function sucyc_v3() {
    $oarr = array(3 => '*', 5 => '#', 15 => '*#');

    for ($i = 0; $i <= 100; ++ $i) {
        foreach ($oarr as $n => $o) {
            if ($i % $n == 0) {
                // echo $o;
            }
        }
    }

    // echo "\n";
}

// 0.3 - 0.4
function sucyc_v4() {
    for ($i = 0; $i <= 100; ++ $i) {
    }
    // echo "\n";
}

// 0.6-0.7
// 输出顺序不对，但速度却很快。
// 还是这个算法更接近答案，但需要解决顺序问题。
function sucyc_v5() {
    
    for ($i = 0; $i <= 100/3; ++ $i) {
        $n3 = 3 * $i;
        $n5 = 5 * $i;
        $n15 = 15 * $i;
        
        if ($n15 <= 100) {
            echo "[{$n15}]*#";
        } else if ($n5 <= 100) {
            echo "[{$n5}]#";
        } else if ($n3 <= 100) {
            echo "[{$n3}]*";
        }
    }
    echo "\n";
}

// 
function sucyc_v6() {
    for ($i = 0; $i <= 100; ++ $i) {
        
    }
    // echo "\n";
}

function run_sucyc()
{
    $times = 1;// 100000;
    $btime = microtime(true);
    for ($i = 0; $i < $times; $i ++) {
        sucyc_v5();
    }
    $etime = microtime(true);
    $dtime = $etime - $btime;

    echo "Used time: {$dtime} sec\n";
}

run_sucyc();


