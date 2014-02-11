<?php
/*
  参考：
  http://blog.csdn.net/wdxin1322/article/details/16826331
  http://www.lanceyan.com/tech/arch/simhash_hamming_distance_similarity.html
  http://blog.sina.com.cn/s/blog_4f27dbd501013ysm.html
 */

function vechash($v)
{
    if (1) {
        $x = ord($v[0]) << 7;
        $m = 1000003;

        $mask = gmp_sub(gmp_pow("2", 64),1);
        $len = strlen($v);
        for ($i = 0; $i < $len; $i++) {
            $x = gmp_and(gmp_xor(gmp_mul($x, $m), ord($v[$i])), $mask);
        }
        $x = gmp_xor($x, $len);
        if(intval(gmp_strval($x)) == -1) {
            $x = -2;
        }
        return gmp_intval($x);
    }

    if (1) {
        $b64 = base64_encode($v);
        return crc32($b64);
    }

    if (1) {
        return crc32($v);
    }

    return -1;
}


function simhash($str)
{
    $slen = strlen($str);
    $cnum = $slen / 3;
    $sh_arr = array_fill(0, 64, 0); // 64个元素

    for ($i = 0; $i < $cnum; $i++) {
        $mbch = substr($str, $i * 3, 3);
        // echo "$mbch,\n";
        $vh = vechash($mbch);

        for ($j = 0; $j < 64; $j ++) {
            $th = (1 << (64 - $j) & $vh) >> (64 - $j);
            if ($th == 1) {
                $sh_arr[$j] += 1;
            } else {
                $sh_arr[$j] += -1;
            }
        }
    }
    // print_r($sh_arr);

    $sh_val = 0;
    for ($i = 0; $i < 64; $i++) {
        if ($sh_arr[$i] > 0) {
            $sh_arr[$i] = 1;
            $sh_val = $sh_val | (1 << (64 - $i));
        } else {
            $sh_arr[$i] = 0;
        }
    }
    // $sh_str = implode('', $sh_arr);
    // echo "'$sh_str' \n";

    return $sh_val;
}

function ham_dist($sh1, $sh2)
{
    $xor_val = $sh1 ^ $sh2;
    $dist = 0;

    while ($xor_val > 0) {
        $xor_val = $xor_val & ($xor_val - 1);
        $dist ++;
    }

    return $dist;
}



function test()
{
    $vh = vechash('噢');
    var_dump($vh);
    
    $s1 = "你妈妈叫你回家吃饭拉哈回家罗回家了" ;
    $s2 = "你的妈妈叫你回家吃饭拉哈回家罗回家拉" ;

    $s1 = "为此我们需要一种应对于海量数据场景的去重方案";
    $s2 = "最后形成去掉噪音词的单词序列并为每个词加上权";

    $sh1 = simhash($s1);
    echo "sh1 = $sh1 \n";
    $sh2 = simhash($s2);
    echo "sh2 = $sh2 \n";

    $times = 1000000;
    $btime = microtime(true);
    for ($i = 0; $i < $times; $i ++) {
        $dist = ham_dist($sh1, $sh2);
        // echo "dist = $dist \n";
    }
    echo "dist = $dist \n";
    $etime = microtime(true);
    $dtime = $etime - $btime;
    $speed = $times / $dtime;
    echo "dtime = $dtime, speed = $speed / sec \n";
}

test();
