use std::io;
use std::io::Write;
use std::sync::mpsc;
use std::thread;
use std::sync::mpsc::Sender;
use std::cmp;

fn get_value(from:&String, pattern:&str) -> i32 {
  for part in from.split(",") {
    if part.contains(pattern){
      return part.split(" ")
        .last().expect("ALSO INVALID")
        .trim()
        .parse().expect("NAN");
    }
  }
  panic!("Invalid Value!!!!");
}

fn obtain_input(tx:&Sender<&str>){
  let mut input = String::new();
  io::stdin().read_line(&mut input).expect("Failed to read line");

  if !input.contains("Event") { return; }
  let event_type   =  get_value(&input,"Event") ;
  let number = get_value(&input,"number") ;
  let value  =  get_value(&input,"value") ;

  if      event_type == 2 && number == 0 && value <  0 { tx.send("<").expect("Failed to send"); }
  else if event_type == 2 && number == 0 && value >  0 { tx.send(">").expect("Failed to send"); }
  else if event_type == 2 && number == 0 && value == 0 { tx.send("=").expect("Failed to send"); }
  else if event_type == 1 && number == 0 && value == 1 { tx.send(".").expect("Failed to send"); }
}


struct Context{
  surface : [ [ [ u8; 3 ]; 256 ]; 256 ],
  position: i32, 
  direction:i32,
  bullet:Option<(i32,i32)>,
  alive : [ bool; 25 ]
}


fn clear( context:&mut Context  ){
  for row in  0..255 { 
    for col in 0..255 { 
      context.surface[row][  col ][ 0 ] = 0x00;  
    } 
  }
}


fn block( surface:&mut [ [ [ u8; 3 ]; 256 ]; 256 ], row:i32, col:i32, size:i32  ){
  for r   in  ( row - size ) .. ( row + size ) { 
    for c in  ( col - size ) .. ( col + size ) { 
      surface[ r as usize ][ c as usize ][ 0 ] = 0xFF;  
    } 
  }
}

fn main() {
  let (tx, rx) = mpsc::channel();
  thread::spawn( move || { loop { obtain_input(&tx); } });
  let mut context:Context = Context{ 
    surface: [ [ [ 128; 3 ]; 256 ]; 256 ], position: 128, 
    direction:0,bullet:None,alive:[true; 25] 
  };
  loop{
    io::stdout().write_all(context.surface.as_flattened().as_flattened()).expect("asdasd");
    clear(&mut context);
 
    match rx.try_recv(){ 
      Ok(input) => {
        if input == ">" { context.direction = 1; }
        else if input == "<" { context.direction = -1; }
        else if input == "=" { context.direction = 0; }
        else  {
          context.bullet = match context.bullet { None => Some( ( 220, context.position ) ), x=>x  };
        }
      }, _=>{}  
    };

    context.position = cmp::max( cmp::min( context.position+context.direction, 226),0);
    context.bullet = match context.bullet { 
      Some((10,_)) => None , 
      Some((row,col)) =>  collision(&mut context.alive,(row-1,col)) , 
      _=> None  
    };


    for (index,alive) in context.alive.iter().enumerate() {
      if !alive {continue;}
      let row = ( 40 + ( index / 5 ) * 20 ).try_into().unwrap();
      let col = ( 40 + ( index % 5 ) * 20 ).try_into().unwrap();
      block( &mut context.surface, row, col, 8 );
    }

    block(&mut context.surface,230,context.position,10);
    match context.bullet { Some(pos) => { block(&mut context.surface,pos.0,pos.1,2)   }, _=>{}  };
  }
}

fn collision (alives:&mut [bool], (brow,bcol):(i32,i32)) -> Option<(i32,i32)> {
    for (index,alive) in alives.iter().enumerate() {
      if !alive {continue;}
      let row:i32 = ( 40 + ( index / 5 ) * 20 ).try_into().unwrap();
      let col:i32 = ( 40 + ( index % 5 ) * 20 ).try_into().unwrap();
      if (brow-row)*(brow-row)+(bcol-col)*(bcol-col) > 50 { continue; }
      alives[ index ] = false;
      return None;
    }
    return Some((brow,bcol));
}
