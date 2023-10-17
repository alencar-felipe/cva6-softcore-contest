import gdb

from tqdm import tqdm

class SimpleBt(gdb.Command):
    def __init__(self):
        super(SimpleBt, self).__init__('simple-bt', gdb.COMMAND_USER)

    def invoke(self, arg, from_tty):
        frame = gdb.newest_frame()
        bt_list = []
        while frame:
            if frame.name():
                bt_list.append(frame.name())
            frame = frame.older()
        bt_list.reverse()
        print(';'.join(bt_list))

    
SimpleBt()

class SoftRecord(gdb.Command):
    def __init__(self):
        super(SoftRecord, self).__init__('soft-record', gdb.COMMAND_USER)

    def invoke(self, arg, from_tty):
        file = open(arg, 'w')
        pbar = tqdm(unit='instr', desc='Instructions per Second')

        try:
            while gdb.lookup_symbol('main')[0]:
                gdb.execute('stepi', to_string=True)
                pbar.update(1)
                pc = gdb.selected_frame().pc()
                simple_bt = gdb.execute('simple-bt', to_string=True).strip()
                file.write(f'0x{pc:08x} {simple_bt}\n')
        except Exception as e:
            raise e
        finally:
            file.close()
            pbar.close()

SoftRecord()

