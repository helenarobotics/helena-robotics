class Message{
	private final String msg;
	private final Requester rqs;

	public Message(String msg, Requester rqs){
		this.msg = msg;
		this.rqs = rqs;
	}

	public String getMessage(){
		return msg;
	}

	public Requester getRequester(){
		return rqs;
	}
}