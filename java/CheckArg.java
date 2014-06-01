import java.util.*;
import java.util.concurrent.*;

public class CheckArg {

	public CheckArg(String args[], final String appname) {
		_args = args;
		_appname = appname;
		_autohelp_on = false;
		_usage_line = _appname + " [options]";

		_valid_args      = new HashMap<String,Boolean>();
		_valid_args_cb   = new HashMap<String,Callable<Void>>();
		_valid_args_vals = new HashMap<String,String>();
		_autohelp        = new HashMap<String,String>();
		_short2long      = new HashMap<Character,String>();
		_next_is_val_of  = "";
		_pos_args        = new ArrayList<String>();

		_descr = "";
		_appendix = "";
		_posarg_help_descr = "";
		_posarg_help_usage = "";
	}
	public CheckArg(String args[], final String appname, final String desc ) {
		this(args,appname);
		_descr = desc;
	}
	public CheckArg(String args[], final String appname, final String desc, final String appendix ) {
		this(args,appname,desc);
		_appendix = appendix;
	}

	public int add(final char sopt, final String lopt, final String help){
		return add(sopt,lopt,help,false);
	}
	public int add(final char sopt, final String lopt, final String help, final boolean has_val){
		_valid_args.put(lopt, has_val);
		_autohelp.put(lopt, help);
		_short2long.put(sopt,lopt);
		return Error.CA_ALLOK.ordinal();
	}

	public int add(final char sopt, final String lopt, final String help, final Callable<Void> cb){
		return add(sopt,lopt,help,cb,false);
	}
	public int add(final char sopt, final String lopt, final String help, final Callable<Void> cb, final boolean has_val){
		add(sopt, lopt, help, has_val);
		_valid_args_cb.put(lopt,cb);
		return Error.CA_ALLOK.ordinal();
	}

	public int add(final String lopt, final String help){
		return add(lopt,help,false);
	}
	public int add(final String lopt, final String help, final boolean has_val){
		_valid_args.put(lopt, has_val);
		_autohelp.put(lopt, help);
		return Error.CA_ALLOK.ordinal();
	}

	public int add(final String lopt, final String help, final Callable<Void> cb){
		return add(lopt,help,cb,false);
	}
	public int add(final String lopt, final String help, final Callable<Void> cb, final boolean has_val){
		add(lopt,help,has_val);
		_valid_args_cb.put(lopt, cb);
		return Error.CA_ALLOK.ordinal();
	}

	public int add_autohelp(){
		add('h', "help", "show this help message and exit", new ShowAutohelp(), false);
		_autohelp_on = true;
		return 0;
	}

	public int parse(){
		int ret;
		
		for (String a : _args){
			ret = arg(a);
			if( ret != Error.CA_ALLOK.ordinal() )
				return ret;
		}
		if( ! _next_is_val_of.isEmpty() ){
			ca_error(Error.CA_MISSVAL, ": "+_args[_args.length-1]+"!");
			return Error.CA_MISSVAL.ordinal();
		}
		return Error.CA_ALLOK.ordinal();
	}

	public void set_posarg_help(final String usage, final String descr){
		_posarg_help_descr = descr;
		_posarg_help_usage = usage;
	}

	public void set_usage_line(final String usage){
		_usage_line = usage;
	}

	public String argv0(){
		return _appname;
	}

	public List<String> pos_args(){
		return _pos_args;
	}

	public String value(final String arg){
		return _valid_args_vals.get(arg);
	}

	public boolean isset(final String arg){
		return _valid_args_vals.get(arg) != null;
	}

	public static String str_err(final Error errno){
		return _errors.get(errno);
	}
	public static void ca_error(Error eno, final String info){
		System.err.println("Error: "+_errors.get(eno.ordinal())+(info==null?"!":info));
	}

	private class ShowAutohelp implements Callable<Void> {
		public Void call(){
			int space = 0;
			for( String l : _valid_args.keySet() )
				space = Math.max(space, l.length());
			space += 2; // 2 more spaces for gap

			System.out.println("Usage: " + _usage_line + " " + _posarg_help_usage);

			if( ! _descr.isEmpty() )
				System.out.println("\n"+_descr);

			System.out.println("\nOptions:");
			for ( String l : _valid_args.keySet() ) {
				String sarg = long2short(l);
				if(!sarg.isEmpty())
					System.out.print("   -"+sarg+",");
				else
					System.out.print("      ");

				System.out.println(" --"+l+str_repeat(' ', space-l.length())+_autohelp.get(l));
			}

			if( ! _posarg_help_descr.isEmpty() ){
				System.out.println("\nPositional Arguments:");
				System.out.println(_posarg_help_descr);
			}

			if( ! _appendix.isEmpty() ){
				System.out.println("\n"+_appendix);
			}

			System.exit(0);

			return null; // always exit after showing help
		}

	}
	
	private String str_repeat(char c, int times){
		char[] chars = new char[times];
		Arrays.fill(chars, c);
		return new String(chars);
	}

	private int arg(final String arg){
		if(! _next_is_val_of.isEmpty() ){
			_valid_args_vals.put(_next_is_val_of, arg);
			int ret = _call_cb(_next_is_val_of);
			_next_is_val_of = "";
			return ret;
		}

		if( arg.charAt(0) == '-' ) { // yay, some arg
			if(arg.charAt(1) == '-' ) // it's a long one
				return arg_long(arg.substring(2));

			// short one or group of short ones
			return arg_short(arg.substring(1));
		}

		// some positional arg
		_pos_args.add(arg);
		return Error.CA_ALLOK.ordinal();
	}

	private int arg_long(final String arg){
		int eqpos = arg.indexOf('=');
		String real_arg, val;
		if( eqpos == -1 ){
			real_arg = arg;
			val = "";
		} else {
			real_arg = arg.substring(0,eqpos);
			val = arg.substring(eqpos+1);
		}

		Boolean hasval = _valid_args.get(real_arg);
		if( hasval != null ){
			if( hasval.booleanValue() && !val.isEmpty() ){
				// has val and val is given after an '='
				_valid_args_vals.put(real_arg, val);
			} else if( hasval.booleanValue()) {
				// value of arg is the next arg, remember that for the next call of arg
				_next_is_val_of = real_arg;
			} else { // arg has no value 
				if( ! val.isEmpty() ){
					ca_error(Error.CA_INVVAL, ": --"+real_arg+"!");
					return Error.CA_INVVAL.ordinal();
				}
				_valid_args_vals.put(real_arg, "x"); // arg was seen
			}

			if( ! hasval.booleanValue() || ! val.isEmpty()){
				// if arg has no val, or val is found already, call callback now, if there's one
        return _call_cb(real_arg);
			}

			return Error.CA_ALLOK.ordinal();
		}
		ca_error(Error.CA_INVARG, ": --"+real_arg+"!");
		return Error.CA_INVARG.ordinal();
	}

	private int arg_short(final String arg){
		int len = arg.length();

		for (int i = 0; i < len; i++) {
			String larg = _short2long.get(arg.charAt(i));
			if( larg != null ){
				if( _valid_args.get(larg).booleanValue() ){ // if has val
					if( i < len-1 ){
						_valid_args_vals.put(larg, arg.substring(i+1));
					} else { // next arg is val
						_next_is_val_of = larg;
					}
					return Error.CA_ALLOK.ordinal();
				} else {
					_valid_args_vals.put(larg, "x");
					int ret = _call_cb(larg);
					if( ret != Error.CA_ALLOK.ordinal() )
					  return ret;
				}
			} else {
				ca_error(Error.CA_INVARG, ": -"+arg.charAt(i)+"!");
				return Error.CA_INVARG.ordinal();
			}
		}
		return Error.CA_ALLOK.ordinal();
	}
	
  private int _call_cb(final String larg){
		Callable<Void> cb = _valid_args_cb.get(larg);
		if( cb != null ){
			try{
				cb.call();
			}catch(Exception e){
				ca_error(Error.CA_CALLBACK, ": "+e+"!");
				e.printStackTrace();
				return Error.CA_CALLBACK.ordinal();
			}
		}
		return Error.CA_ALLOK.ordinal();
  }

	private String long2short(final String larg){
		for (Map.Entry<Character,String> e : _short2long.entrySet() ) {
			if( e.getValue().equals(larg) )
				return Character.toString(e.getKey());
		}
		return null;
	}

	private String _appname, _descr, _appendix, _usage_line, _posarg_help_descr, _posarg_help_usage;
	private boolean _autohelp_on;
	private String _next_is_val_of;

	private Map<String,Boolean>        _valid_args;
	private Map<String,String>         _valid_args_vals;
	private Map<String,Callable<Void>> _valid_args_cb;
	private Map<String,String>         _autohelp;
	private Map<Character,String>      _short2long;
	private List<String>               _pos_args;
	private String                     _args[];

	private static final Map<Error, String> _errors;
	static {
		_errors = new HashMap<Error,String>();
		_errors.put(Error.CA_ALLOK,    "Everything is fine");
		_errors.put(Error.CA_ERROR,    "An Error occurred");
		_errors.put(Error.CA_INVARG,   "Unknown command line argument");
		_errors.put(Error.CA_INVVAL,   "Value given to non-value argument");
		_errors.put(Error.CA_MISSVAL,  "Missing value of argument");
		_errors.put(Error.CA_CALLBACK, "Callback returned with error code");
	}

	public enum Error {
		CA_ALLOK,
		CA_ERROR,
		CA_INVARG,
		CA_INVVAL,
		CA_MISSVAL,
		CA_CALLBACK,
	}
}
