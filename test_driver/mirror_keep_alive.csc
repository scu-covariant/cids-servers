import network as net
import bitwise

context.add_literal("hex", bitwise.hex_literal)
constant bitset = bitwise.bitset

var server_ep = net.tcp.endpoint("192.168.233.14", 20802)
var sock = new net.tcp.socket
var ip = "192.168.233.181"

function send_keep_alive(cpu_load, mem_load, net_load)
    var header = new bitset
    block
        var arr = ip.split({'.'})
        foreach it in arr
            header = header.shift_left(8).logic_or(bitset.from_number(it.to_number()))
        end
        header = header.shift_left(32)
    end
    header = header.logic_or(bitset.from_number(cpu_load).shift_left(24))
    header = header.logic_or(bitset.from_number(mem_load).shift_left(16))
    header = header.logic_or(bitset.from_number(net_load).shift_left(8))
    system.out.println(header.to_string())
    var header_str = new string
    foreach i in range(8)
        header_str += char.from_ascii(header.logic_and("0xFF"hex).to_number())
        header = header.shift_right(8)
    end
    sock.send(header_str)
end

sock.connect(server_ep)

loop
    send_keep_alive(127, 127, 127)
    runtime.delay(1000)
end