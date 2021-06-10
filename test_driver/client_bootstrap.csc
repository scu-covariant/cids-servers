import network as net
import bitwise

context.add_literal("hex", bitwise.hex_literal)
constant bitset = bitwise.bitset

var server_ep = net.udp.endpoint("192.168.233.14", 20800)
# 7-Byte
var uuid = "0x46a8e4d9515650e4"hex.shift_left(8)
# 1-Byte, 2~254
var reconnect_time = 0

function send_bootstrap()
    var header = "12345670"
    var sock = new net.udp.socket
    sock.open_v4()
    foreach i in range(10) do sock.send_to(header, server_ep)
    try
        return runtime.wait_until(100, sock.receive_from, {128, server_ep})
    catch e
        if reconnect_time < 254
            ++reconnect_time
        end
        return null
    end
end

loop
    var ip = send_bootstrap()
    if ip != null
        system.out.println("Get: " + ip + ", Reconnect Time: " + reconnect_time)
        break
    else
        system.out.println("failed get ip")
    end
end