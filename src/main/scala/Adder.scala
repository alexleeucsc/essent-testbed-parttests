package playground

import Chisel._
import Chisel.iotesters.{PeekPokeTester, Driver}


class Adder(val w: Int) extends Module {
  val io = new Bundle {
    val in0 = UInt(INPUT,  w)
    val in1 = UInt(INPUT,  w)
    val out = UInt(OUTPUT, w)
  }
  val x = "h0".U
  io.out := io.in0 + io.in1
}

class AdderTests(c: Adder) extends PeekPokeTester(c) {
  for (i <- 0 until 10) {
    val in0 = rnd.nextInt(1 << c.w)
    val in1 = rnd.nextInt(1 << c.w)
    poke(c.io.in0, in0)
    poke(c.io.in1, in1)
    step(1)
    expect(c.io.out, (in0 + in1)&((1 << c.w)-1))
  }
}

object AdderMain {
  def main(args: Array[String]): Unit = {
    if (args.size > 0) {
      if (!Driver(() => new Adder(16), "firrtl")(c => new AdderTests(c))) System.exit(1)
    } else {
      if (!Driver.run(() => new Adder(16), "./test_run_dir/playground.Adder/Adder")(c => new AdderTests(c))) System.exit(1)
    }
  }
}